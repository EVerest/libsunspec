file(GLOB SUNSPEC_MODELS_FILES sunspec_models/json/*.json)
install(FILES ${SUNSPEC_MODELS_FILES} DESTINATION sunspec/models)
