CHECK_ERROR(fileSize_ < INIT_DATA_SIZE                 , "not enough data"     , NO_DATA);

CHECK_ERROR(program_[0] != 'T' || program_[1] != 'O' ||
            program_[2] != 'Y' || program_[3] != '1'   , "version is incorrect", INCORRECT_VERSION);

CHECK_ERROR(!data_[CODE_SZ]                            , "code size is 0"      , CODE_SZ_ZERO);

CHECK_ERROR((data_[CDATA] && data_[SMC]  && 
             data_[DATA]  && data_[MEM]) &&
            (data_[CDATA] >= data_[SMC]   || 
             data_[SMC]   >= data_[DATA]  ||
             data_[DATA]  >= data_[MEM])               , "memory is incorrect" , MEMORY_INCORRECT);

CHECK_ERROR(data_[CDATA] && !data_[CDATA_SZ]           , "cdata field error"   , CDATA_FIELD_ERROR);

CHECK_ERROR(data_[DATA] && !data_[DATA_SZ]             , "data field error"    , DATA_FIELD_ERROR);
