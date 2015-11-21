ping = `ping j.tivvit.cz -c 1 | grep time= | awk '{print $8}' | sed s/time=//g | tr '\n' ' '`
#ping_status = `ping google.com -c 1`
#status = `echo $? | tr '\n' ' '`
status = 0
if ping == ""
	status = 1
end
timestamp = Time.now.to_i
puts "#{timestamp}\t#{status}\t#{ping}"
