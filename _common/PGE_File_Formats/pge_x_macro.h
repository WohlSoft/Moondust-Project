#ifndef PGE_X_MACRO_H
#define PGE_X_MACRO_H

#define PGEX_FileBegin() int str_count=0; /*Line Counter*/\
                         PGESTRING line;  /*Current Line data*/

#define PGEX_FileParseTree(raw)  PGEFile pgeX_Data(raw);\
                            if( !pgeX_Data.buildTreeFromRaw() )\
                            {\
                                errorString = pgeX_Data.lastError();\
                                goto badfile;\
                            }
#define PGEX_FetchSection() for(int section=0; section<pgeX_Data.dataTree.size(); section++)
#define PGEX_FetchSection_begin() PGEFile::PGEX_Entry &f_section = pgeX_Data.dataTree[section];\
                                  if(f_section.name=="") continue;

#define PGEX_Section(sct)   else if(f_section.name==sct)
#define PGEX_SectionBegin(stype) if(f_section.type!=stype) \
{ \
    errorString=PGESTRING("Wrong section data syntax:\nSection ["+f_section.name+"]");\
    goto badfile;\
}

#define PGEX_Items() for(int sdata=0;sdata<(signed)f_section.data.size();sdata++)
#define PGEX_ItemBegin(stype) if(f_section.data[sdata].type!=stype) \
{ \
    errorString=PGESTRING("Wrong data item syntax:\nSection ["+f_section.name+"]\nData line "+fromNum(sdata));\
    goto badfile;\
}\
PGEFile::PGEX_Item x = f_section.data[sdata];


#define PGEX_Values() for(int sval=0;sval<(signed)x.values.size();sval++)
#define PGEX_ValueBegin()  PGEFile::PGEX_Val v = x.values[sval];\
                           errorString=PGESTRING("Wrong value syntax\nSection ["+f_section.name+ \
                           "]\nData line "+fromNum(sdata) \
                           +"\nMarker "+v.marker+"\nValue "+v.value);\
                           if(v.marker=="") continue;

#define PGEX_StrVal(Mark, targetValue)  else if(v.marker==Mark) { if(PGEFile::IsQStr(v.value)) \
                                                targetValue = PGEFile::X2STR(v.value); \
                                                else goto badfile; }

#define PGEX_StrArrVal(Mark, targetValue)  else if(v.marker==Mark) { if(PGEFile::IsStringArray(v.value)) \
                                                targetValue = PGEFile::X2STRArr(v.value); \
                                                else goto badfile; }

#define PGEX_BoolVal(Mark, targetValue)  if(v.marker==Mark) { if(PGEFile::IsBool(v.value)) \
                                         targetValue = (bool)toInt(v.value);\
                                         else goto badfile; }

#define PGEX_BoolArrVal(Mark, targetValue)  else if(v.marker==Mark) { if(PGEFile::IsBoolArray(v.value)) \
                                             targetValue = PGEFile::X2BollArr(v.value); \
                                            else goto badfile; }

#define PGEX_UIntVal(Mark, targetValue)  if(v.marker==Mark) { if(PGEFile::IsIntU(v.value)) \
                                         targetValue = toInt(v.value);\
                                         else goto badfile; }

#define PGEX_SIntVal(Mark, targetValue)  if(v.marker==Mark) { if(PGEFile::IsIntS(v.value)) \
                                         targetValue = toInt(v.value);\
                                         else goto badfile; }

#define PGEX_FloatVal(Mark, targetValue)  if(v.marker==Mark) { if(PGEFile::IsFloat(v.value)) \
                                          targetValue = toDouble(v.value);\
                                          else goto badfile; }


#endif // PGE_X_MACRO_H

