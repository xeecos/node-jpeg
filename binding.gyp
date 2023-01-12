{
  "targets": [
    {
        "target_name": "demo",
        "sources": [ "demo.cc"],
        "include_dirs" : [
            "<!(node -e \"require('nan')\")"
        ]
    }
  ]
}