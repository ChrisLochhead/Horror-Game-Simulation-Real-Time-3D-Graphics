#include "Material.h"

Material::Material(int type)
{
	if (type == 1)
	{
		material = type1;
	}
	if (type == 2)
	{

		material = type2;
	}
	if (type == 3)
	{

		material = type3;
	}
	if (type == 4)
	{
		material = type4;
	}
}