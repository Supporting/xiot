#include "X3DDataTypeFactory.h"
#include <sstream>

using namespace std;

#define isWhiteSpace(c) ((c) == ' ' || (c) == '\t' || (c) == '\v' ||  (c) == '\n' || (c) == '\r' || (c) == '\f') 
#define isWhiteSpaceOrComma(c) (isWhiteSpace((c)) || ((c) == ','))

namespace XIOT {

	bool X3DDataTypeFactory::getSFBoolFromString(const std::string &s){
		std::stringstream ss;
		bool b;
		
		ss << s;
		ss >> std::boolalpha >> b;

		return b;
	}

	float X3DDataTypeFactory:: getSFFloatFromString(const std::string &s){
		std::stringstream ss;
		float f;
		
		ss << s;
		ss >> f;

		return f;
	}

	int X3DDataTypeFactory::getSFInt32FromString(const std::string &s){
		std::stringstream ss;
		int i;
		
		ss << s;
		ss >> i;

		return i;
	}

	SFVec3f X3DDataTypeFactory::getSFVec3fFromString(const std::string &s){
		std::stringstream ss;
		SFVec3f vec;
		
		ss << s;
		ss >> vec.x >> vec.y >> vec.z;

		return vec;
	}

	SFVec2f X3DDataTypeFactory::getSFVec2fFromString(const std::string &s){
		std::stringstream ss;
		SFVec2f vec;
		
		ss << s;
		ss >> vec.x >> vec.y;

		return vec;
	}

	SFRotation X3DDataTypeFactory::getSFRotationFromString(const std::string &s){
		std::stringstream ss;
		SFRotation rot;
		
		ss << s;
		ss >> rot.x >> rot.y >> rot.z >> rot.angle;

		return rot;
	}

	std::string X3DDataTypeFactory::getSFStringFromString(const std::string &s){
		return s;
	}

	SFColor X3DDataTypeFactory::getSFColorFromString(const std::string &s){
		std::stringstream ss;
		SFColor col;
		
		ss << s;
		ss >> col.r >> col.g >> col.b;

		return col;
	}

	SFColorRGBA X3DDataTypeFactory::getSFColorRGBAFromString(const std::string &s){
		std::stringstream ss;
		SFColorRGBA col;
		
		ss << s;
		ss >> col.r >> col.g >> col.b >> col.a;

		return col;
	}

	SFImage X3DDataTypeFactory::getSFImageFromString(const std::string &s){
		std::stringstream ss;
		SFImage img;
		int	index = 0;

		ss << s;

		// in case of a parsing error, ss.fail() will return true
		while(!(ss.eof() || ss.fail()))
			ss >> img[index++];	

		return img;
	} 

	// Multi Field
	std::vector<float> X3DDataTypeFactory::getMFFloatFromString(const std::string &s){
		std::vector<float> vec;
		std::stringstream ss;
		
		float fTemp;
		 
		ss << s;

		// in case of a parsing error, ss.fail() will return true
		while(!(ss.eof() || ss.fail()))
		{
			ss >> fTemp;
			
			// look for WS or ',' and skip it
			int c = ss.peek();
			while(isWhiteSpaceOrComma(c))
			{
				ss.ignore(1);
				c = ss.peek();
			}
			vec.push_back(fTemp);
		}		

		return vec;
	}

	std::vector<int> X3DDataTypeFactory::getMFInt32FromString(const std::string &s){
		std::vector<int> vec;

		std::stringstream ss;
		int iTemp;

		ss << s;

		// in case of a parsing error, ss.fail() will return true
		while(!(ss.eof() || ss.fail()))
		{
			ss >> iTemp;

			int c = ss.peek();
			while(isWhiteSpaceOrComma(c))
			{
				ss.ignore(1);
				c = ss.peek();
			}
			
			vec.push_back(iTemp);
		}		

		return vec;
	}

	std::vector<SFVec3f> X3DDataTypeFactory::getMFVec3fFromString(const std::string &s){
		std::vector<SFVec3f> vec;
		std::stringstream ss;
		
		SFVec3f tempVec;
		 
		ss << s;

		// in case of a parsing error, ss.fail() will return true
		while(!(ss.eof() || ss.fail()))
		{

			ss >> tempVec.x >> tempVec.y >> tempVec.z;
			
			int c = ss.peek();
			while(isWhiteSpaceOrComma(c))
			{
				ss.ignore(1);
				c = ss.peek();
			}
						
			vec.push_back(tempVec);
		}		
		
		return vec;
	}

	std::vector<SFVec2f> X3DDataTypeFactory::getMFVec2fFromString(const std::string &s){
		std::vector<SFVec2f> vec;
		std::stringstream ss;
		
		SFVec2f tempVec;
		char c;
		 
		ss << s;

		// in case of a parsing error, ss.fail() will return true
		while(!(ss.eof() || ss.fail()))
		{
			ss >> tempVec.x >> tempVec.y;
			
			c = ss.peek();	// look for ',' and skip it
			if(c == ',')
				ss.ignore(1);

			vec.push_back(tempVec);
		}		
		
		return vec;
	}

	std::vector<SFRotation> X3DDataTypeFactory::getMFRotationFromString(const std::string &s){
		std::vector<SFRotation> vec;
		std::stringstream ss;
		SFRotation tempRot;
		char c;
	 
		ss << s;

		// in case of a parsing error, ss.fail() will return true
		while(!(ss.eof() || ss.fail()))
		{
			ss >> tempRot.x >> tempRot.y >> tempRot.z >> tempRot.angle;
			
			c = ss.peek();	// look for ',' and skip it
			if(c == ',')
				ss.ignore(1);
			
			vec.push_back(tempRot);
		}		
		
		return vec;
	}

	std::vector<std::string> X3DDataTypeFactory::getMFStringFromString(const std::string &s){
		std::vector<std::string> vec;

		std::string tempString;
		std::stringstream ss;
		char c;

		ss << s;

		// in case of a parsing error, ss.fail() will return true
		while(!(ss.eof() || ss.fail()))
		{
			ss >> tempString;
			
			c = ss.peek();	// look for ',' and skip it
			if(c == ',')
				ss.ignore(1);
			
			vec.push_back(tempString);
		}		

		return vec;
	}

	std::vector<SFColor> X3DDataTypeFactory::getMFColorFromString(const std::string &s){
		std::vector<SFColor> vec;
		std::stringstream ss;
		
		SFColor tempColor;
		 
		ss << s;

		// in case of a parsing error, ss.fail() will return true
		while(!(ss.eof() || ss.fail()))
		{
			ss >> tempColor.r >> tempColor.g >> tempColor.b;
			
			// look for ',' and skip it
			int c = ss.peek();
			while(isWhiteSpaceOrComma(c))
			{
				ss.ignore(1);
				c = ss.peek();
			}
			vec.push_back(tempColor);
		}		
		
		return vec;
	}

	std::vector<SFColorRGBA> X3DDataTypeFactory::getMFColorRGBAFromString(const std::string &s){
		std::vector<SFColorRGBA> vec;
		std::stringstream ss;
		
		SFColorRGBA tempColor;
		 
		ss << s;

		// in case of a parsing error, ss.fail() will return true
		while(!(ss.eof() || ss.fail()))
		{
			ss >> tempColor.r >> tempColor.g >> tempColor.b >> tempColor.a;
			
			// look for ',' and skip it
			int c = ss.peek();
			while(isWhiteSpaceOrComma(c))
			{
				ss.ignore(1);
				c = ss.peek();
			}
			vec.push_back(tempColor);
		}		
		
		return vec;
	}

}

