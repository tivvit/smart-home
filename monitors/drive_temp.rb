timestamp = Time.now.to_i

drives =  `fdisk -l 2>&1 | grep "Disk /dev/sd" | awk '{print $2}' | sed 's/://g' | tr '\n' ' '`
drives = drives.split()

for index in 0 ... drives.size
        #puts drives[index]
        drive_temp = `hddtemp #{drives[index]} | sed 's/://g' |  tr '\n' ' '`
        drive_id = `hdparm -I #{drives[index]} | grep "Serial Num" | awk '{print $3}'`
        puts "#{timestamp} #{drive_temp} #{drive_id}"
end
