#include "DxApp.h"
class TokenStream
{
   public:
      TokenStream( );

      void ResetStream( );

      void SetTokenStream( char* data );
      bool GetNextToken( std::string* buffer, char* delimiters, int totalDelimiters );
      bool MoveToNextLine( std::string *buffer );

   private:
      int startIndex_, endIndex_;
      std::string data_;
};