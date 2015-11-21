df_res = `df | grep -E 'md2|rootfs'`
timestamp = Time.now.to_i
df_res.split(/\r?\n/).each do |line|
	puts "#{timestamp} #{line}"
end 
