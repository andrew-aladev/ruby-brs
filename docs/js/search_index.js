var search_data = {"index":{"searchIndex":["brs","accessioerror","allocateerror","baseerror","decompressorcorruptedsourceerror","file","notenoughdestinationbuffererror","notenoughsourcebuffererror","option","readioerror","stream","raw","compressor","option","decompressor","option","reader","writer","string","option","validation","writeioerror","compress()","compress()","get_compressor_options()","get_compressor_options()","get_compressor_options()","get_compressor_options()","get_decompressor_options()","get_decompressor_options()","get_decompressor_options()","get_decompressor_options()","native_compress_io()","native_compress_string()","native_decompress_io()","native_decompress_string()","new()","validate_bool()","authors","license","readme"],"longSearchIndex":["brs","brs::accessioerror","brs::allocateerror","brs::baseerror","brs::decompressorcorruptedsourceerror","brs::file","brs::notenoughdestinationbuffererror","brs::notenoughsourcebuffererror","brs::option","brs::readioerror","brs::stream","brs::stream::raw","brs::stream::raw::compressor","brs::stream::raw::compressor::option","brs::stream::raw::decompressor","brs::stream::raw::decompressor::option","brs::stream::reader","brs::stream::writer","brs::string","brs::string::option","brs::validation","brs::writeioerror","brs::file::compress()","brs::string::compress()","brs::option::get_compressor_options()","brs::option::get_compressor_options()","brs::option::get_compressor_options()","brs::option::get_compressor_options()","brs::option::get_decompressor_options()","brs::option::get_decompressor_options()","brs::option::get_decompressor_options()","brs::option::get_decompressor_options()","brs::file::native_compress_io()","brs::string::native_compress_string()","brs::file::native_decompress_io()","brs::string::native_decompress_string()","brs::stream::raw::compressor::new()","brs::validation::validate_bool()","","",""],"info":[["BRS","","BRS.html","","<p>Ruby bindings for brotli library. Copyright © 2019 AUTHORS, MIT License.\n"],["BRS::AccessIOError","","BRS/AccessIOError.html","",""],["BRS::AllocateError","","BRS/AllocateError.html","",""],["BRS::BaseError","","BRS/BaseError.html","",""],["BRS::DecompressorCorruptedSourceError","","BRS/DecompressorCorruptedSourceError.html","",""],["BRS::File","","BRS/File.html","","<p>BRS::File class.\n"],["BRS::NotEnoughDestinationBufferError","","BRS/NotEnoughDestinationBufferError.html","",""],["BRS::NotEnoughSourceBufferError","","BRS/NotEnoughSourceBufferError.html","",""],["BRS::Option","","BRS/Option.html","","<p>BRS::Option module.\n"],["BRS::ReadIOError","","BRS/ReadIOError.html","",""],["BRS::Stream","","BRS/Stream.html","",""],["BRS::Stream::Raw","","BRS/Stream/Raw.html","",""],["BRS::Stream::Raw::Compressor","","BRS/Stream/Raw/Compressor.html","","<p>BRS::Stream::Raw::Compressor class.\n"],["BRS::Stream::Raw::Compressor::Option","","BRS/Option.html","","<p>BRS::Option module.\n"],["BRS::Stream::Raw::Decompressor","","BRS/Stream/Raw/Decompressor.html","","<p>BRS::Stream::Raw::Decompressor class.\n"],["BRS::Stream::Raw::Decompressor::Option","","BRS/Option.html","","<p>BRS::Option module.\n"],["BRS::Stream::Reader","","BRS/Stream/Reader.html","","<p>BRS::Stream::Reader class.\n"],["BRS::Stream::Writer","","BRS/Stream/Writer.html","","<p>BRS::Stream::Writer class.\n"],["BRS::String","","BRS/String.html","","<p>BRS::String class.\n"],["BRS::String::Option","","BRS/Option.html","","<p>BRS::Option module.\n"],["BRS::Validation","","BRS/Validation.html","","<p>BRS::Validation class.\n"],["BRS::WriteIOError","","BRS/WriteIOError.html","",""],["compress","BRS::File","BRS/File.html#method-c-compress","(source, destination, options = {})","<p>Compresses data from <code>source</code> file path to <code>destination</code> file path. Option: <code>:source_buffer_length</code> source …\n"],["compress","BRS::String","BRS/String.html#method-c-compress","(source, options = {})","<p>Compresses <code>source</code> string using <code>options</code>. Option: <code>:destination_buffer_length</code> destination buffer length. …\n"],["get_compressor_options","BRS::Option","BRS/Option.html#method-c-get_compressor_options","(options, buffer_length_names)","<p>Processes compressor <code>options</code> and <code>buffer_length_names</code>. Option: <code>:source_buffer_length</code> source buffer length. …\n"],["get_compressor_options","BRS::Option","BRS/Option.html#method-c-get_compressor_options","(options, buffer_length_names)","<p>Processes compressor <code>options</code> and <code>buffer_length_names</code>. Option: <code>:source_buffer_length</code> source buffer length. …\n"],["get_compressor_options","BRS::Option","BRS/Option.html#method-c-get_compressor_options","(options, buffer_length_names)","<p>Processes compressor <code>options</code> and <code>buffer_length_names</code>. Option: <code>:source_buffer_length</code> source buffer length. …\n"],["get_compressor_options","BRS::Option","BRS/Option.html#method-c-get_compressor_options","(options, buffer_length_names)","<p>Processes compressor <code>options</code> and <code>buffer_length_names</code>. Option: <code>:source_buffer_length</code> source buffer length. …\n"],["get_decompressor_options","BRS::Option","BRS/Option.html#method-c-get_decompressor_options","(options, buffer_length_names)","<p>Processes decompressor <code>options</code> and <code>buffer_length_names</code>. Option: <code>:source_buffer_length</code> source buffer length. …\n"],["get_decompressor_options","BRS::Option","BRS/Option.html#method-c-get_decompressor_options","(options, buffer_length_names)","<p>Processes decompressor <code>options</code> and <code>buffer_length_names</code>. Option: <code>:source_buffer_length</code> source buffer length. …\n"],["get_decompressor_options","BRS::Option","BRS/Option.html#method-c-get_decompressor_options","(options, buffer_length_names)","<p>Processes decompressor <code>options</code> and <code>buffer_length_names</code>. Option: <code>:source_buffer_length</code> source buffer length. …\n"],["get_decompressor_options","BRS::Option","BRS/Option.html#method-c-get_decompressor_options","(options, buffer_length_names)","<p>Processes decompressor <code>options</code> and <code>buffer_length_names</code>. Option: <code>:source_buffer_length</code> source buffer length. …\n"],["native_compress_io","BRS::File","BRS/File.html#method-c-native_compress_io","(*args)","<p>Bypass native compress.\n"],["native_compress_string","BRS::String","BRS/String.html#method-c-native_compress_string","(*args)","<p>Bypasses native compress.\n"],["native_decompress_io","BRS::File","BRS/File.html#method-c-native_decompress_io","(*args)","<p>Bypass native decompress.\n"],["native_decompress_string","BRS::String","BRS/String.html#method-c-native_decompress_string","(*args)","<p>Bypasses native decompress.\n"],["new","BRS::Stream::Raw::Compressor","BRS/Stream/Raw/Compressor.html#method-c-new","(options = {})","<p>Initializes compressor. Option: <code>:destination_buffer_length</code> destination buffer length. Option: <code>:size_hint</code> …\n"],["validate_bool","BRS::Validation","BRS/Validation.html#method-c-validate_bool","(value)","<p>Raises error when <code>value</code> is not boolean.\n"],["AUTHORS","","AUTHORS.html","","<p>Andrew Aladjev Jenner La Fave\n"],["LICENSE","","LICENSE.html","","<p>MIT License\n<p>Copyright © 2019 AUTHORS\n<p>Permission is hereby granted, free of charge, to any person obtaining …\n"],["README","","README_md.html","","<p>Ruby bindings for brotli library\n<p>| AppVeyor | Jenkins | Github actions | Codecov | Gem |\n| :------: | ...\n"]]}}