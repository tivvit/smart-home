top_res = `top -bn1 | head -5`
timestamp = Time.now.to_i
top_res.split(/\r?\n/).each do |line|
	puts "#{timestamp} #{line}"
end 
