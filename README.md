# Ruby bindings for brotli library

[![Travis build status](https://travis-ci.org/andrew-aladev/ruby-brs.svg?branch=master)](https://travis-ci.org/andrew-aladev/ruby-brs)
[![AppVeyor build status](https://ci.appveyor.com/api/projects/status/github/andrew-aladev/ruby-brs?branch=master&svg=true)](https://ci.appveyor.com/project/andrew-aladev/ruby-brs/branch/master)

See [brotli library](https://github.com/google/brotli).

## Installation

Please install brotli library first.

```sh
gem install ruby-brs
```

You can build it from source.

```sh
rake gem
gem install pkg/ruby-brs-*.gem
```

## Usage

There are simple APIs: `String` and `File`. Also you can use generic streaming API: `Stream::Writer` and `Stream::Reader`.

```ruby
require "brs"

data = BRS::String.compress "sample string"
puts BRS::String.decompress(data)

BRS::File.compress "file.txt", "file.txt.br"
BRS::File.decompress "file.txt.br", "file.txt"

BRS::Stream::Writer.open("file.txt.br") { |writer| writer << "sample string" }
puts BRS::Stream::Reader.open("file.txt.br") { |reader| reader.read }
```

You can create and read `tar.br` archives with `minitar` for example.

```ruby
require "brs"
require "minitar"

BRS::Stream::Writer.open "file.tar.br" do |writer|
  Minitar::Writer.open writer do |tar|
    tar.add_file_simple "file", :data => "sample string"
  end
end

BRS::Stream::Reader.open "file.tar.br" do |reader|
  Minitar::Reader.open reader do |tar|
    tar.each_entry do |entry|
      puts entry.name
      puts entry.read
    end
  end
end
```

## Options

Each API supports several options:

```
:source_buffer_length
:destination_buffer_length
```

There is internal buffer for compressed and decompressed data.
For example you want to use 1 KB as source buffer length for compressor - please use 256 B as destination buffer length.
You want to use 256 B as source buffer length for decompressor - please use 1 KB as destination buffer length.

Values: 0 - infinity, default value: 0.
0 means automatic buffer length selection.

```
:mode
```

Values: [`:text`, `:font`, `:generic`], default value: `:generic`.
Please read brotli docs for more info.

```
:quality
```

Values: 0 - 11, default value: 11.
Please read brotli docs for more info.

```
:lgwin
```

Values: 10 - 24, default value: 22.
Please read brotli docs for more info.

```
:lgblock
```

Values: 16 - 24, default value: none.
Please read brotli docs for more info.

```
:disable_literal_context_modeling
```

Values: true/false, default value: false.
Please read brotli docs for more info.

```
:disable_ring_buffer_reallocation
```

Values: true/false, default value: false.
Please read brotli docs for more info.

```
:size_hint
```

Values: 0 - infinity, default value: 0.
Please read brotli docs for more info.

```
:large_window
```

Values: true/false, default value: false.
Please read brotli docs for more info.

Possible compressor options:
```
:mode
:quality
:lgwin
:lgblock
:disable_literal_context_modeling
:size_hint
:large_window
```

Possible decompressor options:
```
:disable_ring_buffer_reallocation
:large_window
```

Example:

```ruby
require "brs"

data = BRS::String.compress "sample string", :quality => 5
puts BRS::String.decompress(data, :disable_ring_buffer_reallocation => true)
```

Default options are compatible with HTTP data processing (`Content-Encoding: br`):

```ruby
require "brs"
require "sinatra"

get "/" do
  headers["Content-Encoding"] = "br"
  BRS::String.compress "sample string"
end
```

## License

MIT license, see LICENSE and AUTHORS.
