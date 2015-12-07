/*!
 * \file pge_x_macro.h
 *
 * \brief Contains helper macroses for making PGE-X format based parsers
 *
 */
#ifndef PGE_X_MACRO_H
#define PGE_X_MACRO_H

/*! \def PGEX_FileBegin()
    \brief Placing at begin of the parsing function
*/
#define PGEX_FileBegin() int str_count=0; /*Line Counter*/\
                         PGESTRING line;  /*Current Line data*/

/*! \def PGEX_FileParseTree(raw)
    \brief Parse PGE-X Tree from raw data
*/
#define PGEX_FileParseTree(raw)  PGEFile pgeX_Data(raw);\
                            if( !pgeX_Data.buildTreeFromRaw() )\
                            {\
                                errorString = pgeX_Data.lastError();\
                                goto badfile;\
                            }

/*! \def PGEX_FetchSection()
    \brief Prepare to fetch all data from specified section
*/
#define PGEX_FetchSection() for(int section=0; section<(signed)pgeX_Data.dataTree.size(); section++)
/*! \def PGEX_FetchSection_begin()
    \brief Prepare to detect separate data of different sections
*/
#define PGEX_FetchSection_begin() PGEFile::PGEX_Entry &f_section = pgeX_Data.dataTree[section];\
                                  if(f_section.name=="") continue;
/*! \def PGEX_Section(sct)
    \brief Defines block of fields for section of specified name
*/
#define PGEX_Section(sct)   else if(f_section.name==sct)
/*! \def PGEX_SectionBegin(stype)
    \brief Run syntax of raw data in this section for specified data type
*/
#define PGEX_SectionBegin(stype) if(f_section.type!=stype) \
{ \
    errorString=PGESTRING("Wrong section data syntax:\nSection ["+f_section.name+"]");\
    goto badfile;\
}
/*! \def PGEX_Items()
    \brief Prepare to read items from this section
*/
#define PGEX_Items() for(int sdata=0;sdata<(signed)f_section.data.size();sdata++)
/*! \def PGEX_ItemBegin(stype)
    \brief Declares block with a list of values
*/
#define PGEX_ItemBegin(stype) if(f_section.data[sdata].type!=stype) \
{ \
    errorString=PGESTRING("Wrong data item syntax:\nSection ["+f_section.name+"]\nData line "+fromNum(sdata));\
    goto badfile;\
}\
PGEFile::PGEX_Item x = f_section.data[sdata];

/*! \def PGEX_Values()
    \brief Declares block with a list of values
*/
#define PGEX_Values() for(int sval=0;sval<(signed)x.values.size();sval++)
/*! \def PGEX_ValueBegin()
    \brief Initializes getting of the values
*/
#define PGEX_ValueBegin()  PGEFile::PGEX_Val v = x.values[sval];\
                           errorString=PGESTRING("Wrong value syntax\nSection ["+f_section.name+ \
                           "]\nData line "+fromNum(sdata) \
                           +"\nMarker "+v.marker+"\nValue "+v.value);\
                           if(v.marker=="") continue;

/*! \def PGEX_StrVal(Mark, targetValue)
    \brief Parse Plain text string value by requested Marker and write into target variable
*/
#define PGEX_StrVal(Mark, targetValue)  else if(v.marker==Mark) { if(PGEFile::IsQStr(v.value)) \
                                                targetValue = PGEFile::X2STR(v.value); \
                                                else goto badfile; }
/*! \def PGEX_StrArrVal(Mark, targetValue)
    \brief Parse Plain text string array value by requested Marker and write into target variable
*/
#define PGEX_StrArrVal(Mark, targetValue)  else if(v.marker==Mark) { if(PGEFile::IsStringArray(v.value)) \
                                                targetValue = PGEFile::X2STRArr(v.value); \
                                                else goto badfile; }

/*! \def PGEX_BoolVal(Mark, targetValue)
    \brief Parse boolean flag value by requested Marker and write into target variable
*/
#define PGEX_BoolVal(Mark, targetValue)  if(v.marker==Mark) { if(PGEFile::IsBool(v.value)) \
                                         targetValue = (bool)toInt(v.value);\
                                         else goto badfile; }

/*! \def PGEX_BoolArrVal(Mark, targetValue)
    \brief Parse boolean flags array value by requested Marker and write into target variable
*/
#define PGEX_BoolArrVal(Mark, targetValue)  else if(v.marker==Mark) { if(PGEFile::IsBoolArray(v.value)) \
                                             targetValue = PGEFile::X2BollArr(v.value); \
                                            else goto badfile; }

/*! \def PGEX_UIntVal(Mark, targetValue)
    \brief Parse unsigned integer value by requested Marker and write into target variable
*/
#define PGEX_UIntVal(Mark, targetValue)  if(v.marker==Mark) { if(PGEFile::IsIntU(v.value)) \
                                         targetValue = toInt(v.value);\
                                         else goto badfile; }

/*! \def PGEX_SIntVal(Mark, targetValue)
    \brief Parse signed integer value by requested Marker and write into target variable
*/
#define PGEX_SIntVal(Mark, targetValue)  if(v.marker==Mark) { if(PGEFile::IsIntS(v.value)) \
                                         targetValue = toInt(v.value);\
                                         else goto badfile; }

/*! \def PGEX_FloatVal(Mark, targetValue)
    \brief Parse floating point value by requested Marker and write into target variable
*/
#define PGEX_FloatVal(Mark, targetValue)  if(v.marker==Mark) { if(PGEFile::IsFloat(v.value)) \
                                          targetValue = toDouble(v.value);\
                                          else goto badfile; }


#endif // PGE_X_MACRO_H

