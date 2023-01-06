{
  "targets": [
    {
        "target_name": "jpeg",
        "sources": [ "jpeg.cc","toojpeg.cpp" ],
        "include_dirs" : [
            "<!(node -e \"require('nan')\")"
        ]
    }
  ]
}