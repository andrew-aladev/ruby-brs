# Ruby bindings for brotli library

| AppVeyor | Jenkins | Github actions | Codecov | Gem  |
| :------: | :-----: | :------------: | :-----: | :--: |
| [![AppVeyor test status](https://ci.appveyor.com/api/projects/status/github/andrew-aladev/ruby-brs?branch=master&svg=true)](https://ci.appveyor.com/project/andrew-aladev/ruby-brs/branch/master) | [![Jenkins test status](http://37.187.122.190:58182/buildStatus/icon?job=ruby-brs)](http://37.187.122.190:58182/job/ruby-brs) | [![Github Actions test status](https://github.com/andrew-aladev/ruby-brs/workflows/test/badge.svg?branch=master)](https://github.com/andrew-aladev/ruby-brs/actions) | [![Codecov](https://codecov.io/gh/andrew-aladev/ruby-brs/branch/master/graph/badge.svg)](https://codecov.io/gh/andrew-aladev/ruby-brs) | [![Gem](https://img.shields.io/gem/v/ruby-brs.svg)](https://rubygems.org/gems/ruby-brs) |

See [brotli library](https://github.com/google/brotli).

## Installation

Operating systems: GNU/Linux, FreeBSD, OSX, Windows (MinGW).

Dependencies: [brotli](https://github.com/google/brotli) 1.0.0+ version.

| Popular OS | Dependencies              |
|------------|---------------------------|
| Ubuntu     | `libbrotli-dev`           |
| CentOS     | `brotli-devel`            |
| ArchLinux  | `brotli`                  |
| OSX        | `brotli`                  |
| Windows    | `mingw-w64-x86_64-brotli` |

```sh
gem install ruby-brs
```

You can build it from source.

```sh
rake gem
gem install pkg/ruby-brs-*.gem
```

You can also use [overlay](https://github.com/andrew-aladev/overlay) for gentoo.

### Installation in macOS on Apple Silicon

On M1 Macs, Homebrew installs to /opt/homebrew, so you'll need to specify its
include and lib paths when building the native extension for brotli.

```sh
brew install brotli
gem install ruby-brs -- --with-opt-include=/opt/homebrew/include --with-opt-lib=/opt/homebrew/lib
```

You can also configure Bundler to use those options when installing:

```sh
bundle config set build.ruby-brs "--with-opt-include=/opt/homebrew/include --with-opt-lib=/opt/homebrew/lib"
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

writer = BRS::Stream::Writer.new output_socket
begin
  bytes_written = writer.write_nonblock "sample string"
  # handle "bytes_written"
rescue IO::WaitWritable
  # handle wait
ensure
  writer.close
end

reader = BRS::Stream::Reader.new input_socket
begin
  puts reader.read_nonblock(512)
rescue IO::WaitReadable
  # handle wait
rescue ::EOFError
  # handle eof
ensure
  reader.close
end
```

You can create and read `tar.br` archives with [minitar](https://github.com/halostatue/minitar).

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

You can also use `Content-Encoding: br` with [sinatra](http://sinatrarb.com):

```ruby
require "brs"
require "sinatra"

get "/" do
  headers["Content-Encoding"] = "br"
  BRS::String.compress "sample string"
end
```

All functionality (including streaming) can be used inside multiple threads with [parallel](https://github.com/grosser/parallel).
This code will provide heavy load for your CPU.

```ruby
require "brs"
require "parallel"

Parallel.each large_datas do |large_data|
  BRS::String.compress large_data
end
```

## Options

| Option                             | Values     | Default    | Description |
|------------------------------------|------------|------------|-------------|
| `source_buffer_length`             | 0 - inf    | 0 (auto)   | internal buffer length for source data |
| `destination_buffer_length`        | 0 - inf    | 0 (auto)   | internal buffer length for description data |
| `gvl`                              | true/false | false      | enables global VM lock where possible |
| `mode`                             | `MODES`    | `:generic` | compressor mode |
| `quality`                          | 0 - 11     | 11         | compression level |
| `lgwin`                            | 10 - 24    | 22         | compressor window size |
| `lgblock`                          | 16 - 24    | nil (auto) | compressor input block size |
| `npostfix`                         | 0 - 3      | nil (auto) | Recommended number of postfix bits |
| `ndirect`                          | 0 - 120    | nil (auto) | Recommended number of direct distance codes (step 1 << npostfix, max 15 << npostfix) |
| `disable_literal_context_modeling` | true/false | false      | disables literal context modeling format |
| `disable_ring_buffer_reallocation` | true/false | false      | disables ring buffer reallocation |
| `size_hint`                        | 0 - inf    | 0 (auto)   | size of input (if known) |
| `large_window`                     | true/false | false      | enables large window |

There are internal buffers for compressed and decompressed data.
For example you want to use 1 KB as `source_buffer_length` for compressor - please use 256 B as `destination_buffer_length`.
You want to use 256 B as `source_buffer_length` for decompressor - please use 1 KB as `destination_buffer_length`.

`gvl` is disabled by default, this mode allows running multiple compressors/decompressors in different threads simultaneously.
Please consider enabling `gvl` if you don't want to launch processors in separate threads.
If `gvl` is enabled ruby won't waste time on acquiring/releasing VM lock.

`String` and `File` will set `:size_hint` automaticaly.

You can also read brotli docs for more info about options.

| Option     | Related constants |
|------------|-------------------|
| `mode`     | `BRS::Option::MODES` = `%i[text font generic]` |
| `quality`  | `BRS::Option::MIN_QUALITY` = 0, `BRS::Option::MAX_QUALITY` = 11 |
| `lgwin`    | `BRS::Option::MIN_LGWIN` = 10, `BRS::Option::MAX_LGWIN` = 24 |
| `lgblock`  | `BRS::Option::MIN_LGBLOCK` = 16, `BRS::Option::MAX_LGBLOCK` = 24 |
| `npostfix` | `BRS::Option::MIN_NPOSTFIX` = 0, `BRS::Option::MAX_NPOSTFIX` = 3 |
| `ndirect`  | `BRS::Option::MIN_NDIRECT` = 0, `BRS::Option::MAX_NDIRECT` = 120, `BRS::Option::NDIRECT_NPOSTFIX_STEP_BASE` = 1, `BRS::Option::NDIRECT_NPOSTFIX_MAX_BASE` = 15 |

Possible compressor options:
```
:source_buffer_length
:destination_buffer_length
:gvl
:mode
:quality
:lgwin
:lgblock
:npostfix
:ndirect
:disable_literal_context_modeling
:size_hint
:large_window
```

Possible decompressor options:
```
:source_buffer_length
:destination_buffer_length
:gvl
:disable_ring_buffer_reallocation
:large_window
```

Example:

```ruby
require "brs"

data = BRS::String.compress "sample string", :quality => 5
puts BRS::String.decompress(data, :disable_ring_buffer_reallocation => true)
```

## String

String maintains destination buffer only, so it accepts `destination_buffer_length` option only.

```
::compress(source, options = {})
::decompress(source, options = {})
```

`source` is a source string.

## File

File maintains both source and destination buffers, it accepts both `source_buffer_length` and `destination_buffer_length` options.

```
::compress(source, destination, options = {})
::decompress(source, destination, options = {})
```

`source` and `destination` are file pathes.

## Stream::Writer

Its behaviour is similar to builtin [`Zlib::GzipWriter`](https://ruby-doc.org/stdlib/libdoc/zlib/rdoc/Zlib/GzipWriter.html).

Writer maintains destination buffer only, so it accepts `destination_buffer_length` option only.

```
::open(file_path, options = {}, :external_encoding => nil, :transcode_options => {}, &block)
```

Open file path and create stream writer associated with opened file.
Data will be transcoded to `:external_encoding` using `:transcode_options` before compressing.

It may be tricky to use both `:size_hint` and `:transcode_options`. You have to provide size of transcoded input.

```
::new(destination_io, options = {}, :external_encoding => nil, :transcode_options => {})
```

Create stream writer associated with destination io.
Data will be transcoded to `:external_encoding` using `:transcode_options` before compressing.

It may be tricky to use both `:size_hint` and `:transcode_options`. You have to provide size of transcoded input.

```
#set_encoding(external_encoding, nil, transcode_options)
```

Set another encodings, `nil` is just for compatibility with `IO`.

```
#io
#to_io
#stat
#external_encoding
#transcode_options
#pos
#tell
```

See [`IO`](https://ruby-doc.org/core/IO.html) docs.

```
#write(*objects)
#flush
#rewind
#close
#closed?
```

See [`Zlib::GzipWriter`](https://ruby-doc.org/stdlib/libdoc/zlib/rdoc/Zlib/GzipWriter.html) docs.

```
#write_nonblock(object, *options)
#flush_nonblock(*options)
#rewind_nonblock(*options)
#close_nonblock(*options)
```

Special asynchronous methods missing in `Zlib::GzipWriter`.
`rewind` wants to `close`, `close` wants to `write` something and `flush`, `flush` want to `write` something.
So it is possible to have asynchronous variants for these synchronous methods.
Behaviour is the same as `IO#write_nonblock` method.

All nonblock operations for file will raise `EBADF` error on Windows.
Setting file into nonblocking mode is [not available on Windows](https://github.com/ruby/ruby/blob/master/win32/win32.c#L4388).

```
#<<(object)
#print(*objects)
#printf(*args)
#putc(object, encoding: ::Encoding::BINARY)
#puts(*objects)
```

Typical helpers, see [`Zlib::GzipWriter`](https://ruby-doc.org/stdlib/libdoc/zlib/rdoc/Zlib/GzipWriter.html) docs.

## Stream::Reader

Its behaviour is similar to builtin [`Zlib::GzipReader`](https://ruby-doc.org/stdlib/libdoc/zlib/rdoc/Zlib/GzipReader.html).

Reader maintains both source and destination buffers, it accepts both `source_buffer_length` and `destination_buffer_length` options.

```
::open(file_path, options = {}, :external_encoding => nil, :internal_encoding => nil, :transcode_options => {}, &block)
```

Open file path and create stream reader associated with opened file.
Data will be force encoded to `:external_encoding` and transcoded to `:internal_encoding` using `:transcode_options` after decompressing.

```
::new(source_io, options = {}, :external_encoding => nil, :internal_encoding => nil, :transcode_options => {})
```

Create stream reader associated with source io.
Data will be force encoded to `:external_encoding` and transcoded to `:internal_encoding` using `:transcode_options` after decompressing.

```
#set_encoding(external_encoding, internal_encoding, transcode_options)
```

Set another encodings.

```
#io
#to_io
#stat
#external_encoding
#internal_encoding
#transcode_options
#pos
#tell
```

See [`IO`](https://ruby-doc.org/core/IO.html) docs.

```
#read(bytes_to_read = nil, out_buffer = nil)
#eof?
#rewind
#close
#closed?
```

See [`Zlib::GzipReader`](https://ruby-doc.org/stdlib/libdoc/zlib/rdoc/Zlib/GzipReader.html) docs.

```
#readpartial(bytes_to_read = nil, out_buffer = nil)
#read_nonblock(bytes_to_read, out_buffer = nil, *options)
```

See [`IO`](https://ruby-doc.org/core/IO.html) docs.

```
#getbyte
#each_byte(&block)
#readbyte
#ungetbyte(byte)

#getc
#readchar
#each_char(&block)
#ungetc(char)

#lineno
#lineno=
#gets(separator = $OUTPUT_RECORD_SEPARATOR, limit = nil)
#readline
#readlines
#each(&block)
#each_line(&block)
#ungetline(line)
```

Typical helpers, see [`Zlib::GzipReader`](https://ruby-doc.org/stdlib/libdoc/zlib/rdoc/Zlib/GzipReader.html) docs.

## Thread safety

`:gvl` option is disabled by default, you can use bindings effectively in multiple threads.
Please be careful: bindings are not thread safe.
You should lock all shared data between threads.

For example: you should not use same compressor/decompressor inside multiple threads.
Please verify that you are using each processor inside single thread at the same time.

## CI

Please visit [scripts/test-images](scripts/test-images).
See universal test script [scripts/ci_test.sh](scripts/ci_test.sh) for CI.

## License

MIT license, see [LICENSE](LICENSE) and [AUTHORS](AUTHORS).
