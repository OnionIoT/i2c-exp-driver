#!/bin/sh

# include the Onion sh lib
. /usr/lib/onion/lib.sh
bLogEnabled=1
#DBG!

##### pwm-exp #####
# function to set a pwm channel with duty cycle
PwmExpSet () {
	# select the arguments object
	json_select params
	# parse the arguments
	json_get_var channel "channel"
	json_get_var duty "duty"
	json_get_var delay "delay"
	json_get_var freq "frequency"
	if [ "$freq" != "" ]; then
		freq="-f $freq"
	fi

	# check arguments
	if 	[ "$channel" == "" ]; then
		ret="missing channel argument"
	elif [ "$duty" == "" ]; then
		ret="missing duty argument"
	else
		ret=$(/usr/sbin/pwm-exp -q $freq $channel $duty $delay)
	fi
	Log "pwm-exp: set $freq $channel $duty $delay"

	echo "$ret"
}

# function to set a pwm channel with pulse width and period
PwmExpSetPeriod () {
	# select the arguments object
	json_select params
	# parse the arguments object
	json_get_var channel "channel"
	json_get_var pulse "pulse"
	json_get_var period "period"

	# check arguments
	if 	[ "$channel" == "" ]; then
		ret="missing channel argument"
	elif [ "$pulse" == "" ]; then
		ret="missing pulse argument"
	elif [ "$period" == "" ]; then
		ret="missing period argument"
	else
		ret=$(/usr/sbin/pwm-exp -q -p $channel $pulse $period)
	fi
	Log "pwm-exp: set -p $channel $pulse $period"

	echo "$ret"
}

# main pwm-exp function
PwmExpMain () {
	# read the command
	json_get_var cmd "command"

	# specific commands
	case "$cmd" in
		init)
			ret=$(/usr/sbin/pwm-exp -q -i)
			Log "pwm-exp: init, ret is $ret"
		;;
		sleep)
			ret=$(/usr/sbin/pwm-exp -q -s)
			Log "pwm-exp: sleep, ret is $ret"
		;;
		set)
			ret=$(PwmExpSet)
		;;
		set-period)
			ret=$(PwmExpSetPeriod)
		;;
	esac

	# print return message
	if [ "$ret" == "" ]; then
		echo '{"status":"success"}'
	else
		echo '{"status":"failure", "error":"'"$ret"'"}'
	fi
}


##### relay-exp #####
# main relay-exp function
RelayExpMain () {
	# read the command
	json_get_var cmd "command"

	# only command is 'set'	
	if [ "$cmd" == "set" ]
	then
		# select the arguments object
		json_select params
		# parse the arguments object
		json_get_var channel "channel"
		json_get_var state "state"
		json_get_var addr "address"
		if [ "$addr" != "" ]; then
			addr="-s $addr"
		fi

		# check arguments
		if 	[ "$channel" == "" ]; then
			ret="missing channel argument"
		elif [ "$state" == "" ]; then
			ret="missing state argument"
		else
			ret=$(/usr/sbin/relay-exp -q $addr $channel $state)
		fi
		Log "relay-exp: set $addr $channel $state"
	else
		ret="invalid command"
	fi
	
	# print return message
	if [ "$ret" == "" ]; then
		echo '{"status":"success", "channel":"'"$channel"'", "state":"'"$state"'"}'
	else
		echo '{"status":"failure", "error":"'"$ret"'"}'
	fi
}


##### oled-exp #####
# main oled-exp function
OledExpMain () {
	# read the command
	json_get_var cmd "command"

	# only command is 'set'	
	if [ "$cmd" == "set" ]
	then
		# parse the options
		json_get_var opts "option"
		if [ "$opts" != "" ]; then
			opts="-$opts"
		fi
		# parse the arguments object
		json_get_keys param "params"
		if [ "$param" != "" ]; then
			cmds=$(_ParseArgumentsObject "nodash")
		fi

		Log "relay-exp: opts: $opts, cmds: $cmds"
		Log "/usr/sbin/oled-exp -q $opts $cmds"
		#ret=$(/usr/sbin/oled-exp -q $opts $cmds)
		eval "/usr/sbin/oled-exp -q $opts $cmds"
	else
		ret="invalid command"
	fi
	
	# print return message
	if [ "$ret" == "" ]; then
		cmds=$(echo $cmds | sed -e "s/\"/'/g")
		echo '{"status":"success", "options":"'"$opts"'", "commands":"'"$cmds"'"}'
	else
		echo '{"status":"failure", "error":"'"$ret"'"}'
	fi
}


########################
##### Main Program #####

cmdPwmExp="pwm-exp"
cmdRelayExp="relay-exp"
cmdOledExp="oled-exp"
cmdStatus="status"

jsonPwmExp='"'"$cmdPwmExp"'": { "command":"cmd", "params": {"key": "value" } }'
jsonRelayExp='"'"$cmdRelayExp"'": { "command":"cmd", "params": {"key": "value" } }'
jsonOledExp='"'"$cmdOledExp"'": { "command":"cmd", "option":"opt", "params": {"key": "value" } }'
jsonStatus='"'"$cmdStatus"'": { }'


case "$1" in
    list)
		echo "{ $jsonPwmExp, $jsonRelayExp, $jsonOledExp, $jsonStatus }"
    ;;
    call)
		Log "Function: call, Method: $2"

		case "$2" in
			$cmdPwmExp)
				# read the json arguments
				read input;
				Log "Json argument: $input"

				# parse the json
				json_load "$input"

				# parse the json and run the function
				PwmExpMain
			;;
			$cmdRelayExp)
				# read the json arguments
				read input;
				Log "Json argument: $input"

				# parse the json
				json_load "$input"

				# parse the json and run the function
				RelayExpMain
			;;
			$cmdOledExp)
				# read the json arguments
				read input;
				Log "Json argument: $input"

				# parse the json
				json_load "$input"

				# parse the json and run the function
				OledExpMain
			;;
			$cmdStatus)
				# dummy call for now
				echo '{"status":"good"}'
		;;
		esac
    ;;
esac

# take care of the log file
CloseLog