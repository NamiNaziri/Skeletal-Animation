#include "Animator.h"

Animator::Animator(Skeleton* skeleton, AnimationClip& animClip, double startTime) :
	skeleton(skeleton),  startTimeForCurrentAnim(startTime), currentClip(&animClip)
{

	currentClipName = animClip.GetName();
	currentTime = startTime;
}

void Animator::ChangeAnimationClip(AnimationClip& animClip, double startTime)
{
	currentClip = &animClip;

	currentClipName = animClip.GetName();
	startTimeForCurrentAnim = startTime;
	currentTime = startTime; //todo is this necessary or is it better to not change it?
	
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
		glm::mat4 final = trans * rot ;
		//TODO scale
		b->SetTransform(final);
	}
	
}
