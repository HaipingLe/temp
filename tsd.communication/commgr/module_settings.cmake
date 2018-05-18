# Preprocessor settings
SET(${MODULE_NAME_UPPER}_DEPENDENCIES "tsd.common" "tsd.communication.comclient")

SET(${MODULE_NAME_UPPER}_INCLUDES "${MODULE_PATH}/src")	# weiterzugebene includes
SET(${MODULE_NAME_UPPER}_DEFINITIONS "") # praeprozessorzeugs

# Linker settings
SET (${MODULE_NAME_UPPER}_LINK_LIBRARIES ${MODULE_NAME})
