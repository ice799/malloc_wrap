source = File.open(ARGV[0], File::RDONLY)

callocs = 0
reallocs = 0
mallocs = 0
more = 0

amounts = { 0 => 0,
            1 => 0,
            2 => 0,
            4 => 0,
            8 => 0,
            16 => 0,
            32 => 0,
            64 => 0,
            128 => 0,
            256 => 0,
            512 => 0,
            1024 => 0,
            2048 => 0,
            4096 => 0,
            8192 => 0,
            16384 => 0,
            32768 => 0,
            65536 => 0,
            131072 => 0 }

def is_power_of_2 arg
  !((arg-1) & arg);
end

def round_up arg
  arg -= 1
  arg |= arg >> 1
  arg |= arg >> 2
  arg |= arg >> 4
  arg |= arg >> 8
  arg |= arg >> 16
  arg += 1
end

begin
  while (line = source.readline)
    if line =~ /malloc.*/
      amount = line[/malloc\((\d*)\)/, 1].to_i
      mallocs += 1
    elsif line =~ /realloc/
      amount = line[/realloc\(.*\,\s(\d*)\)/,1].to_i
      reallocs += 1
    elsif line =~ /calloc/
      amount = line[/calloc\((\d*)\,\s(\d*)\)/,1].to_i * line[/calloc\((\d*)\,\s(\d*)\)/,2].to_i
      callocs += 1
    else
      next
    end
   
    if is_power_of_2 amount
      amounts[amount] += 1
    elsif (round_up amount) <= 131072
      amounts[round_up amount] += 1
    else
      more += 1
    end
  end
rescue EOFError
  source.close
  puts "mallocs: #{mallocs}"
  puts "callocs: #{callocs}"
  puts "reallocs: #{reallocs}"
  puts "more: #{more}"
  so_far = 0.0
  total = mallocs + callocs + reallocs
  amounts.sort_by {|k,v| k}.each { |k,v| 
    so_far += v
    puts "#{(so_far/total)*100}% of allocs are <= #{k}\n"
  }
  amounts.sort_by {|k,v| k}.each { |k,v| puts "#{k} => #{v}" }
  puts "more than 131072: #{more}"
end
