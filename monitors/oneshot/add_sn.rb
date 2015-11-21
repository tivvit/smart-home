text=File.open('/scylla/stats/hdd_temp_stats.txt').read
text.gsub!(/\r\n?/, "\n")

sn = Hash.new

text.each_line do |line|
    drive = line.split()[1]
    if !sn.has_key?(drive)
        sn[drive] = `hdparm -I #{drive} 2>&1 | grep "Serial Num" | awk '{print $3}'`
        if sn[drive] == ""
                sn[drive] = "?"
        end
    end
    line.gsub!("\n", "")
    puts "#{line} #{sn[drive]}"
end