#ifndef __CSMIPRINTERMACRO_H__
#define __CSMIPRINTERMACRO_H__

// --------------------------------------------------------------
#define LEVEL(level) \
   ((level) == 0) ? "" : ( \
   ((level) == 1) ? "> " : ( \
   ((level) == 2) ? "  > " : ( \
   ((level) == 3) ? "    > " : ( \
   ((level) == 4) ? "      > " : ( \
   ((level) == 5) ? "        > " : ( \
   ((level) == 6) ? "          > " : ( \
   ((level) == 7) ? "            > " : ( \
   ((level) == 8) ? "              > " : ( \
   ((level) == 9) ? "                > " : ( \
      "----------" ))))))))))

#define DEF_BUFFER_1(buf1) char buf1[128]
#define DEF_BUFFER_2(buf1, buf2) char buf1[128], buf2[128]
#define DEF_BUFFER_3(buf1, buf2, buf3) char buf1[128], buf2[128], buf3[128]
#define DEF_BUFFER_4(buf1, buf2, buf3, buf4) char buf1[128], buf2[128], buf3[128], buf4[128]

#define RETURN_NAME(name) return name

// [level] name   :
#define PRINT_NAME(level, name) \
   do{ \
      DEF_BUFFER_1(codeBuf); \
      sprintf(codeBuf, "%s" "%s", LEVEL(level), name); \
      sstr << codeBuf << endl; \
   }while(0)

// [level] field   : value    range   comment
#define PRINT_VALUE_INT(info, level, field) \
   do{ \
      DEF_BUFFER_2(nameBuf, codeBuf); \
      sprintf(nameBuf, "%s" "%s", LEVEL(level), #field); \
      sprintf(codeBuf, "%-36s" ": %u", nameBuf, (U32)(info.field)); \
      sstr << codeBuf << endl; \
   }while(0)

// [level] field   : value    range   comment
#define PRINT_VALUE_STRING(info, level, field) \
   do{ \
      DEF_BUFFER_2(nameBuf, codeBuf); \
      sprintf(nameBuf, "%s" "%s", LEVEL(level), #field); \
      sprintf(codeBuf, "%-36s" ": %s", nameBuf, info.field); \
      sstr << codeBuf << endl; \
   }while(0)

// [level] field   : value    range   comment
#define PRINT_VALUE_ENUM(info, level, field, tostringfunc) \
   do{ \
      DEF_BUFFER_2(nameBuf, codeBuf); \
      sprintf(nameBuf, "%s" "%s", LEVEL(level), #field); \
      sprintf(codeBuf, "%-36s" ": %s", nameBuf, CSMIPrinter::tostringfunc(info.field).c_str()); \
      sstr << codeBuf << endl; \
   }while(0)

// [level] field   : value    range   comment
#define PRINT_VALUE_STRUCT(info, level, field) \
   do{ \
      DEF_BUFFER_2(nameBuf, codeBuf); \
      sprintf (nameBuf, "%s%s", LEVEL(level), #field); \
      sprintf (codeBuf, "%-36s#%s ", nameBuf, CSMIPrinter::ToTypeString(info.field).c_str()); \
      sstr << codeBuf << endl << CSMIPrinter::ToString(info.field, (level + 1)); \
   }while(0)

// --------------------------------------------------------------

// [level] field   : value    range   comment
#define PRINT_INT_ITEM(info, level, field, index) \
   do{ \
      DEF_BUFFER_2(nameBuf, codeBuf); \
      sprintf(nameBuf, "%s%s[%d]", LEVEL(level), #field, index); \
      sprintf(codeBuf, "%-36s" ": %d", nameBuf, info.field[index]); \
      sstr << codeBuf << endl; \
   }while(0)

// [level] field   : value    range   comment
#define PRINT_STRUCT_ITEM(info, level, field, index) \
   do{ \
      DEF_BUFFER_2(nameBuf, codeBuf); \
      sprintf (nameBuf, "%s%s[%d]", LEVEL(level), #field, index); \
      sprintf (codeBuf, "%-36s#%s ", nameBuf, CSMIPrinter::ToTypeString(info.field[0]).c_str()); \
      sstr << codeBuf << endl << CSMIPrinter::ToString(info.field[index], (level + 1)) << endl; \
   }while(0)

// --------------------------------------------------------------

#define PRINT_ARRAY_INT_OLD(info, idx, name) \
   do{ \
      int count = sizeof (info.name) / sizeof (info.name[0]); \
      for (int index = 0; index < count; index++) { \
         PRINT_INT_ITEM(info, idx, name, index); \
      } \
   }while(0)

#define PRINT_ARRAY_INT(info, idx, name) \
   do{ \
      DEF_BUFFER_2(nameBuf, codeBuf); \
      int count = sizeof (info.name) / sizeof (info.name[0]); \
      sprintf(nameBuf, "%s%s[%d]", LEVEL(idx), #name, count); \
      sprintf(codeBuf, "%-36s" ": ", nameBuf); \
      sstr << codeBuf; \
      for (int index = 0; index < count; index++) { \
         sstr << (U32) info.name[index] << " "; \
      } \
      sstr << endl; \
   }while(0)

#define PRINT_ARRAY_CHAR(info, idx, name) \
   do{ \
      DEF_BUFFER_2(nameBuf, codeBuf); \
      int count = sizeof (info.name) / sizeof (info.name[0]); \
      sprintf(nameBuf, "%s%s[%d]", LEVEL(idx), #name, count); \
      sprintf(codeBuf, "%-36s" ": ", nameBuf); \
      sstr << codeBuf; \
      for (int index = 0; index < count; index++) { \
         sstr << (char) info.name[index]; \
      } \
      sstr << endl; \
   }while(0)

#define PRINT_ARRAY_STRUCT(info, idx, count, name) \
   do{ \
      int curcount = count; \
      int maxcount = sizeof (info.name) / sizeof (info.name[0]); \
      curcount = (0 == curcount) ? maxcount : MIN2(curcount, maxcount); \
      for (int index = 0; index < curcount; index++) { \
         PRINT_STRUCT_ITEM(info, idx, name, index); \
      } \
   }while(0)

// --------------------------------------------------------------

#endif // __CSMIPRINTERMACRO_H__
