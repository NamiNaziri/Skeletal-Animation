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

void Animator::SetSkeletonPose(AnimationPose& pose)
{
	for (auto ap : pose.keyframesMap)
	{

		Bone* b = skeleton->GetBoneByName(ap.first);

		Bone* root = skeleton->GetBones()[0];
		if(b )
		{
			glm::mat4 rot = glm::toMat4(ap.second.rotation);
			glm::mat4 trans = glm::mat4(1.f);

			if(enableRootMotion)
			{
				trans = glm::translate(glm::mat4(1.0f), ap.second.position);
			}
			else
			{
				if (b != root)
					trans = glm::translate(glm::mat4(1.0f), ap.second.position);
				else
				{
					//trans = glm::translate(glm::mat4(1.0f), glm::vec3(0,ap.second.position.y,0));
				}
			}
			
			
			glm::mat4 final = trans * rot;
			//TODO scale
			b->SetTransform(final);
		}
		
	}
}

double Animator::GetCurrentClipTime()
{
	return GetCurrentClipTime();
}

const AnimationClip& Animator::GetCurrentClip()
{
	return *currentClip;
}

AnimationPose Animator::GetPoseAtCurrentTime()
{
	return currentClip->GetPoseForCurrentFrame((currentTime - startTimeForCurrentAnim) * currentClip->GetFramePerSecond());

}

void Animator::SetRootMotionEnable(bool enable)
{
	enableRootMotion = enable;
}

void Animator::Update(double deltaTime)
{
	currentTime += deltaTime;
	double CurrentAnimationTime = (currentTime - startTimeForCurrentAnim);
	AnimationPose currentPose = currentClip->GetPoseForCurrentFrame(CurrentAnimationTime * currentClip->GetFramePerSecond());
	
	SetSkeletonPose(currentPose);
}



