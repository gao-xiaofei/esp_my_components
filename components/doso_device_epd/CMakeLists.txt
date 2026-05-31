file(GLOB_RECURSE SOURCES	./*.c
								)
								
idf_component_register(SRCS ${SOURCES}
							INCLUDE_DIRS
							.
							REQUIRES
							"doso_driver_spi_software_display"
							)