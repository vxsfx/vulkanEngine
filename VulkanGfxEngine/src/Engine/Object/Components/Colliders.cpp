#include "Colliders.h"

//if hasCollided return true, than set both collided to true, else both false, rather than set internally
//OR SET BOTH INTERNALLY


void Collider::updateCollisions() {
	for (int cur = 0; cur < allColliders.size(); cur++) {
		const auto current = allColliders[cur];
		Collider* other = nullptr;

						//offset, no need to recheck
		for (int oth = cur; oth < allColliders.size(); other++) {
			other = allColliders[oth];
			if (!current->isStatic && !other->isStatic) {
				
				
				/////////////////////////////
				if (current->hasCollided(other)) {
					current->curremtlyCollided = true;
					other->curremtlyCollided = true;
				}
			
			
			
			}

		}
		
	}

}


bool stupidCollider::hasCollided(Collider* other) {
	return true;
}


