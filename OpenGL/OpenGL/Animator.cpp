#include "Animator.h"

Animator::Animator(Skeleton* skeleton, AnimationClip& animClip, double startTime) :
	skeleton(skeleton),  startTimeForCurrentAnim(startTime), currentClip(&animClip)
{
	currentClipName = animClip.GetName();
	currentTime = startTime;
}

void Animator::Update(double deltaTime)
{
	currentTime += deltaTime;
	
	AnimationPose currentPose = currentClip->GetPoseForCurrentFrame((currentTime - startTimeForCurrentAnim) * currentClip->GetFramePerSecond());
	

	for(auto ap: currentPose.keyframesMap)
	{
		
		Bone* b = skeleton->GetBoneByName(ap.first);

		glm::mat4 rot =  glm::toMat4(ap.second.rotation);
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), ap.second.position);
		glm::mat4 final = rot * trans;
		//TODO scale
		b->SetTransform(final);
	}
	
}
