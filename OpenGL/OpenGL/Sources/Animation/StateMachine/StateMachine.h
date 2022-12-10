#pragma once
#include "../Animation.h"
#include "../Animator.h"
#include <functional>
/*class AbstractState
{

public:
	virtual void Update(double deltaTime) = 0;
	//virtual void Transition() = 0;
};

class AnimationAbstractState : public AbstractState
{
protected:
	AnimationClip* animClip;

};*/


/*
 * Note: the state machine is responsible for cleaning up the states (deleting them)
 * Note: the states are responsible for cleaning up the transitions (deleting them)
 */


struct Transition
{
	
public:
	Transition(std::string to, std::function<bool()> evaluateFunction, double transitionTime);
	
	//std::string from;
	std::string to;
	std::function<bool()> evaluateFunction;
	double transitionTime;
	
	bool Evaluate();
};

class AnimationState
{
public:
	AnimationState(std::string stateName, AnimationClip* animClip);
	~AnimationState();
private:
	std::string stateName;
	AnimationClip* animClip;
	//void Update();

private:
	std::vector<Transition*> transitions;
public:
	AnimationClip* GetAnimClip() { return animClip; }
	const std::string& GetStateName() { return stateName; }
	const std::vector<Transition*>& GetTransitions() { return transitions; }
	void AddNewTransition(Transition* newTransition) { transitions.push_back(newTransition); }
};


/*
 * Normal status is when the animation is playing normally
 * Transitioning means it is in middle of the transition, so no evaluation of current state does not need to happen
 * finished is when the transition finished. Therefor, we can change the state
 */


class AnimationStateMachine
{
	enum TransitionStatus { normal, transitioning, finished };
	
private:
	Animator* animator = nullptr;
	AnimationState* currentState = nullptr;

	
	std::map<std::string, AnimationState*> animationStatesMap; // map of Animation state name and the object
	TransitionStatus transitionStatus = TransitionStatus::normal;



	double transitionTime;
	double currentTime;
	AnimationPose TransitionFromPose;
	AnimationPose TransitionToPose;
	
	// returns true if transition is finished 
	bool TransitionUpdate(double deltaTime);
public:
	AnimationStateMachine(Animator* animator, AnimationState* initialState);
	~AnimationStateMachine();

	
	void Update(double deltaTime);

	void AddNewState(AnimationState* animationState);
};

