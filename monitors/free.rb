free_res = `free | grep -E 'Mem|Swap'`
timestamp = Time.now.to_i
free_res.split(/\r?\n/).each do |line|
	puts "#{timestamp} #{line}"
end 
