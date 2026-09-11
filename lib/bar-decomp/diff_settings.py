import os


def apply(config, args):
    basename = "beetleadventurerac"
    version = "us"
    if os.path.exists(f"build/{basename}.jp.bin"):
        version = "jp"

    config["baseimg"] = f"baserom.us.z64"
    config["myimg"] = f"build/{basename}.{version}.z64"
    config["mapfile"] = f"build/{basename}.{version}.map"
    config["source_directories"] = ["src", "include"]
