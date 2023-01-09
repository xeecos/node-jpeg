{
  "targets": [
    {
        "target_name": "jpeg",
        "sources": [ "jpeg.cc","libs/nanojpeg.c","libs/jpge.cpp"],
        "include_dirs" : [
            "<!(node -e \"require('nan')\")"
        ]
    }
  ]
}