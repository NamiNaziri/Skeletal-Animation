#include "StateMachine.h"

Transition::Transition(std::string to, std::function<bool()> evaluateFunction, double transitionTime)
	: to(to), evaluateFunction(std::move(evaluateFunction)), transitionTime(transitionTime){}

bool Transition::Evaluate()
{
	return evaluateFunction();
}

AnimationState::AnimationState(std::string stateName, std::shared_ptr<AnimationClip> animClip)
	:stateName(stateName), animClip(animClip){}

AnimationState::~AnimationState()
{
}

bool AnimationStateMachine::TransitionUpdate(double deltaTime)
{
	currentTime += deltaTime;
	if(currentTime > transitionTime)
	{
		return true;
	}
	else
	{
		//blend
		const float alpha = currentTime / transitionTime;

		AnimationPose pose;
		for (auto x : TransitionFromPose.keyframesMap)
		{
			if(TransitionToPose.keyframesMap.find(x.first) == TransitionToPose.keyframesMap.end())
			{
				// Not found the key
				continue;
			}
			AnimationKeyframe keyframe;
			keyframe.position = glm::mix(x.second.position, TransitionToPose.keyframesMap.at(x.first).position, alpha);
			keyframe.rotation = glm::slerp(x.second.rotation, TransitionToPose.keyframesMap.at(x.first).rotation, alpha);
			keyframe.scale = glm::mix(x.second.scale, TransitionToPose.keyframesMap.at(x.first).scale, alpha);
			pose.keyframesMap.emplace(x.first, keyframe);
		}

		
		animator->SetSkeletonPose(pose);
		return false;
		
	}
}

AnimationStateMachine::AnimationStateMachine(std::shared_ptr<Animator> animator, std::shared_ptr<AnimationState> initialState)
	: animator(animator), currentState(initialState)
{
	AddNewState(initialState);
	animator->ChangeAnimationClip((currentState->GetAnimClip()), 0);
}

AnimationStateMachine::~AnimationStateMachine()
{
}

void AnimationStateMachine::Update(double deltaTime)
{
	// check if any transition has happend
		// get last frame of the current state animation
			// call transition function
			// if transition function has finished go to new state

	if(transitionStatus == TransitionStatus::normal) 
	{
		for (const auto&  transition : currentState->GetTransitions()) // loop through transitions of the current state
		{
			if (transition->Evaluate())
			{
				transitionStatus = TransitionStatus::transitioning;
				currentState = animationStatesMap.at(transition->to);
				TransitionFromPose = animator->GetPoseAtCurrentTime();
				TransitionToPose = currentState->GetAnimClip()->GetPoseForCurrentFrame(0);
				currentTime = 0;
				transitionTime = transition->transitionTime;
				break;
			}
		}
	}

	if (transitionStatus == TransitionStatus::normal)
	{
		animator->Update(deltaTime);
	}
	else if(transitionStatus == TransitionStatus::transitioning)
	{
		if(TransitionUpdate(deltaTime)) 
		{
			transitionStatus = TransitionStatus::finished;
		}
	}
	else if(transitionStatus == TransitionStatus::finished)
	{
		animator->ChangeAnimationClip((currentState->GetAnimClip()), 0); 
		transitionStatus = TransitionStatus::normal;
	}
}

void AnimationStateMachine::AddNewState(std::shared_ptr<AnimationState> animationState)
{
	animationStatesMap.emplace(animationState->GetStateName(), animationState);
}
