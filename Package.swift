import PackageDescription

let package = Package(
    name: "Lzma",
    targets: [
        Target(name: "Lzma", dependencies: ["lzma_wrp"])
    ]
)
