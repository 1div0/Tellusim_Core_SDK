// swift-tools-version: 6.0

import PackageDescription

let package = Package(
	name: "@NAME@",
	platforms: [
		.macOS(.v15)
	],
	targets: [
		.binaryTarget(
			name: "Tellusim",
			path: "./Tellusim.xcframework"
		),
		.executableTarget(
			name: "@TARGET@",
			dependencies: ["Tellusim"],
			path: "Sources",
			linkerSettings: [
				.unsafeFlags(["-L.", "-lTellusimObjC_@ARCH@"], .when(configuration: .release)),
				.unsafeFlags(["-L.", "-lTellusimObjC_@ARCH@d"], .when(configuration: .debug))
			]
		)
	]
)
