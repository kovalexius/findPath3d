
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifdef _MSC_VER
#define noexcept
#endif

#include <locale>

#include <set>
#include <exception>

#include <memory>
#include "Heightmap.h"

//! Читает карту из файла 
/*! Вход:     
 *          filename - имя файла
   Выход:
            height_map - двумерный массив объектов типа Cell */
#ifdef _WIN32
void readMap( const wchar_t* filename, HeightMap &height_map );
#else
void readMap( const char* filename, HeightMap &height_map );
#endif

class custom_exception : std::exception
{
public:
  custom_exception( const std::string& message );
  virtual const char* what() const noexcept override;
protected:
  std::string m_message;
};