
var inputElement =  document.getElementById("input");
inputElement.addEventListener("change", handleFiles, false);

var canvas1 = document.getElementById('canvas1');
var ctx1 = canvas1.getContext('2d');


// handle input file
function handleFiles(e) {
    console.log("Running handleFiles on " + e + ", e.target.result: " + e.target.files[0]);

    var file = e.target.files[0];
    var reader = new FileReader();
    reader.addEventListener("load", processImage, false);
    reader.readAsArrayBuffer(file);
}

// process an a bitmap image
function processImage(e) {
    console.log("Running processImage on " + e + ", e.target.result: " + e.target.result);

    // convert BMP to image data
    var buffer      = e.target.result;
    var bitmap      = getBMP(buffer);
    var imageData   = convertToImageData(bitmap);

    // display on a canvas
    ctx1.putImageData(imageData, 0, 0);

    // convert to a data array
    var dataArray   = convertToDataArray(imageData);

    // convert data array into a hex string
    var output      = generateHexArray(dataArray);

    console.log(output);
}

// process a bitmap header and data
function getBMP(buffer) {
    var datav 		= new DataView(buffer);
    var bitmap 		= {};

    bitmap.fileheader 		            = {};
    bitmap.fileheader.bfType            = datav.getUint16(0, true);     // bitmap type
    bitmap.fileheader.bfSize            = datav.getUint32(2, true);     // bitmap size in bytes
    bitmap.fileheader.bfReserved1 		= datav.getUint16(6, true);     // reserved
    bitmap.fileheader.bfReserved2 		= datav.getUint16(8, true);     // reserved
    bitmap.fileheader.bfOffBits 		= datav.getUint32(10, true);    // starting address of image data
    
    bitmap.infoheader 		            = {};
    bitmap.infoheader.biSize 	        = datav.getUint32(14, true);    // header size
    bitmap.infoheader.biWidth           = datav.getUint32(18, true);    // width in pixels
    bitmap.infoheader.biHeight          = datav.getUint32(22, true);    // height in pixels
    bitmap.infoheader.biPlanes          = datav.getUint16(26, true);    // number of color planes
    bitmap.infoheader.biBitCount 		= datav.getUint16(28, true);    // number of bits per pixel
    bitmap.infoheader.biCompression 	= datav.getUint32(30, true);    // compression method
    bitmap.infoheader.biSizeImage 		= datav.getUint32(34, true);    // image size (raw image data)
    bitmap.infoheader.biXPelsPerMeter 	= datav.getUint32(38, true);    // horizontal resolution (pixel per meter)
    bitmap.infoheader.biYPelsPerMeter 	= datav.getUint32(42, true);    // vertical resolution (pixels per meter)
    bitmap.infoheader.biClrUsed 		= datav.getUint32(46, true);    // number of colors
    bitmap.infoheader.biClrImportant 	= datav.getUint32(50, true);    // number of important collors

    var start 		    = bitmap.fileheader.bfOffBits;
    bitmap.stride 		= Math.floor(
        (bitmap.infoheader.biBitCount *
        bitmap.infoheader.biWidth+31)/32)*4;
    bitmap.pixels 		= new Uint8Array(buffer, start);

    console.log("Read BMP file data, image: " + bitmap.infoheader.biWidth + "x" + bitmap.infoheader.biHeight);

    return bitmap;
}

// read bitmap into a data-array
//  location    data
//  data[i+0]   R
//  data[i+1]   G
//  data[i+3]   B
//  data[i+4]   A
function convertToImageData(bitmap) {
    canvas          = document.createElement("canvas");

    var ctx         = canvas.getContext("2d");
    var Width       = bitmap.infoheader.biWidth;
    var Height      = bitmap.infoheader.biHeight;
    var imageData   = ctx.createImageData(Width, Height);
    var data        = imageData.data;
    var bmpdata     = bitmap.pixels;
    var stride      = bitmap.stride;

    for (var y = 0; y < Height; ++y) {
        for (var x = 0; x < Width; ++x) {
            var index1  = (x + Width*(Height-1-y) ) * 4;
            var index2  = x * 3 + stride * y;

            data[index1]        = bmpdata[index2 + 2];  // Red component
            data[index1 + 1]    = bmpdata[index2 + 1];  // Green component
            data[index1 + 2]    = bmpdata[index2];      // Blue component
            data[index1 + 3]    = 255;                  // Alpha component
        }
    }

    return imageData;
}

// convert an RGB array to a greyscale array
function convertToGrayscale(colorImageData) {
    var colorData       = colorImageData.data;

    console.log("Starting conversion loop, colorData.length = " + colorData.length);
    // convert color to greyscale
    for(var i = 0; i < colorData.length; i += 4) {
        var brightness = 0.34 * colorData[i] + 0.5 * colorData[i + 1] + 0.16 * colorData[i + 2];
        // red
        colorData[i]     = brightness;
        // green
        colorData[i + 1] = brightness;
        // blue
        colorData[i + 2] = brightness;
    }


    // return the grayscale data
    return colorImageData;
}



function convertToDataArray(imageData) {
    var data   = imageData.data;

    console.log("convertToDataArray: ", imageData);

    var cols    = imageData.width;
    var rows    = imageData.height;
    var lcdArr  = [];
    var index   = 0;

    // generate the array
    for (var i = 0; i < rows; i += 8)   // page index
    {
        for (var j = 0; j < cols; j++)    // column index
        {
            // find the current lcd index
            index   = (i * cols)/8 + j;

            // initialize the lcd datapoint
            lcdArr[index]   = 0;
            
            // loop through the rows in this current page+column 
            for (var k = 0; k < 8; k++)       // row index
            {
                //console.log("page: " + i + ", column: " + j + ", bit: " + k + ",   lcd index = " + index);

                // find if this pixel should be represented by black or white
                var val     = colorToBlackOrWhite(imageData, j, i+k);

                // push the b+w value into the byte
                //  lcd byte ordering:
                //      LSB is at the top of the page
                lcdArr[index]   = lcdArr[index] | ((val & 0x01) << k);
            }
        }
    }

    return lcdArr;
}

// convert an RGB color value to black or white
function colorToBlackOrWhite(imageData, x,y)
{
    var index   = (y * imageData.width + x) * 4;
    var data    = imageData.data;
    var colors  = [data[index], data[index+1], data[index+2] ];

    var average=0;
    for (var h=0;h<3;h++)
    {
        if (colors[h]==undefined) {
            colors[h]=255;
        }
        average=average+colors[h];
    }   
    average=average/3;                    
    average=Math.round(average);
    
    if (average>128) {
        // average color is bright, should be white
        return 0; 
    }
    else {
        // average color is dim, should be black
        return 1;
    }
}

// take an array and output it as a hex string
function generateHexArray(dataArr) {
    var hexArray  = "";

    for (var i = 0; i < dataArr.length; i++) {
        hexArray  += "0x";
        hexArray  += dec2hex(dataArr[i]);
        hexArray  += ","
    }

    return hexArray;
}

// convert a decimal into hex
//  including padding for values less than 16
function dec2hex(i)
{
    var result = "00";
    if      (i >= 0    && i <= 15)    { result = "0" + i.toString(16); }
    else if (i >= 16   && i <= 255)   { result =    i.toString(16); }
    return result;
}
