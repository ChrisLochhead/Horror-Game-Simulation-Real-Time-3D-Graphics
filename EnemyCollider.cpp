#include "EnemyCollider.h"

void EnemyCollider::setScale(glm::vec3 newScale)
{
		halfWidth *= newScale.x;
		halfHeight *= newScale.y;
		halfLength *= newScale.z;
}
void EnemyCollider::setTranslate(glm::vec3 newPosition)
{
	centre.x = newPosition.x;
	centre.y = newPosition.y;
	centre.z = newPosition.z;
}


float EnemyCollider::getCentre(int type)
{
	if (type == 0) {

		return centre.x;
	}
	if (type == 1)
	{
		return centre.y;
	}
	if (type == 2)
	{
		return centre.z;
	}
}