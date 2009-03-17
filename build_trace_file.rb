source = File.open(ARGV[0], File::RDONLY)
out = File.open(ARGV[1], File::RDWR|File::CREAT)
@@ident_hash = Hash.new
@@next_id = 1

def lookup addy
  return 0 if addy == 'x0' # null case

  ret = @@ident_hash[addy]
  if ret == nil
    @@ident_hash[addy] = @@next_id
    ret = @@next_id
    @@next_id += 1
  end
  ret
end


begin
  out.puts "                    "  # leave some space for total number of unique ids

  while (line = source.readline)
    if line =~ /(?:(x[\da-f]+) = )?(\w+)\((.+)\)/
      case $2
      when 'free'
        out.puts "f #{lookup $3}"
      when 'malloc'
        out.puts "m #{lookup $1} #{$3}"
      when 'realloc'
        id = $1
        addr, size = $3.split(', ')
        out.puts "r #{lookup id} #{lookup addr} #{size}"
      when 'calloc'
        id = $1
        count, size = $3.split(', ')
        out.puts "c #{lookup id} #{count} #{size}"
      end
    end

    # if line[0,1] == "f"
    #   ident = line[/.*\((.*)\)/, 1]
    #   ident = lookup ident
    #   out.puts "f #{ident}\n"
    #   next
    # end
    #
    # func = line[/.*\s=\s([a-z]*)\(/,1]
    # ident = line[/(.*)\s=/,1]
    #
    # ident = lookup ident
    # if func == "malloc"
    #   size = line[/.*\s=\s[a-z]*\((.*)\)/,1]
    #   out.puts "m #{ident} #{size}\n"
    # elsif func == "realloc"
    #   size = line[/.*\s=\s[a-z]*\(.*,\s(.*)\)/,1]
    #   block = line[/.*\s=\s[a-z]*\((.*),.*\)/,1]
    #   block = lookup block
    #   out.puts "r #{ident} #{block} #{size}\n"
    # elsif func == "calloc"
    #   elm_size = line[/.*\s=\s.*\([\d]*,\s([\d]*)/,1]
    #   num_elm = line[/.*\s=\s.*\(([\d]*),/,1]
    #   out.puts "c #{ident} #{elm_size} #{num_elm}\n"
    # end
  end
rescue EOFError
  out.rewind
  out.puts "#{@@next_id}\n"
  out.close
  source.close
end
