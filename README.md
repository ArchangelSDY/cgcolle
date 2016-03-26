# cgcolle
A GalGame CG archive format, with balance between compression ratio and random access speed

## Background

Here are two common ways to archive CG images:

* Compress each image individually and package them.
* Keep all images in bitmap format, package them and compress it as a single binary stream.

The formal one have bad compression ratio since it doesn't take the similarity between images into account.

The latter one achieves excellent compression ratio while it loses the ability to random access images unless decompress the whole archive.

## Idea

CGColle implements a very simple archive format:

* Images are grouped according to their similarity, by name or by visual look.
* For each group, we produce a main frame and several sub frames, just like some video compression method.
* A sub frame is the XOR result of raw image and main frame.
* Main frame and sub frames are then encoded using PNG.
* All frames are finally packaged into a single archive file, with meta data embedded.

## Performance

Lower than 20% compression ratio, mostly determined by how many similar images in a single group.

Almost const random access time.

## License

MIT.
