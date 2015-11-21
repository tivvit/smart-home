# Pinger
# pings google once
# return: timestamp \t status

ping = system("ping google.com -c 1 > /dev/null")
timestamp = Time.now.to_i
puts "#{timestamp}\t#{$?.exitstatus}"
