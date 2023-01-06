{
  "targets": [
    {
        "target_name": "jpeg",
        "sources": [ "jpeg.cc","libs/toojpeg.cpp","libs/tjpgd.c"],
        "include_dirs" : [
            "<!(node -e \"require('nan')\")"
        ]
    }
  ]
}