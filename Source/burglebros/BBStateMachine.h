// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Queue.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStateMachine, Log, All);

template<typename T>
class IBBStringHelper
{
public:
	virtual FString GetDisplayString(T Value) = 0;
};


// Helper to stringify enum values for state machine debug output
template<typename T>
class TBBEnumStringifier :
	public IBBStringHelper<T>
{
public:
	TBBEnumStringifier(FString InEnumName) :
		EnumName(InEnumName)
	{
	}

	inline virtual FString GetDisplayString(T Value) override
	{
		UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
		ensureMsgf(Enum, TEXT("Could not find enum '%s'"), *EnumName);
		if (Enum != nullptr)
		{
			return Enum->GetNameStringByValue(static_cast<int64>(Value));
		}
		else
		{
			return FString();
		}
	}

	FString EnumName;
	virtual ~TBBEnumStringifier(){ }
};

template<typename TStateType, typename TEventType>
class TBBStateMachine
{
public:
	TBBStateMachine(TStateType InitialState, TEventType InitialEvent)
		: CurrentState(InitialState), InitialEvent(InitialEvent)
	{

	}

	TBBStateMachine(const class TBBStateMachine& Other) = delete;
	TBBStateMachine& operator=(const class TBBStateMachine& Other) = delete;
	~TBBStateMachine()
	{
	}

	typedef TBaseDelegate<void, TStateType, TEventType> OnEnterStateDelegate;
	typedef TBaseDelegate<void, TStateType, TEventType> OnLeaveStateDelegate;
	typedef TBaseDelegate<void, TEventType> OnHandleEventDelegate;
	typedef TBaseDelegate<void, float> OnUpdateState;
	typedef TBaseDelegate<bool> Conditional;


	class BBTransition
	{
	public:
		BBTransition(TStateType InTargetState, TEventType InEvent, Conditional InCanEnter = Conditional()) :
			TargetState(InTargetState),
			TriggeringEvent(InEvent),
			CanEnter(InCanEnter)
		{
		}

		// Target state we want to go to if this transition is triggered
		TStateType TargetState;
		// Event that might trigger this transition
		TEventType TriggeringEvent;

		// Delegate to check if we can enter this transition
		Conditional CanEnter;

		bool ProcessEvent(TEventType Event)
		{
			// Allow transition if this is the triggering event and if CanEnter is not bound or returns true if bound.
			return TriggeringEvent == Event && (!CanEnter.IsBound() || CanEnter.Execute());
		}
	};

	class BBState
	{
	public:
		BBState(TStateType InState) :
			State(InState),
			IgnoresMissingTransitions(false)
		{
		}

		TStateType State;
		OnEnterStateDelegate OnEnterDelegate;
		OnLeaveStateDelegate OnLeaveDelegate;
		OnUpdateState OnUpdateDelegate;
		TMap<TEventType, OnHandleEventDelegate> EventHandlers;

		TArray<BBTransition> Transitions;
		TArray<TEventType> IgnoredEvents;
		bool IgnoresMissingTransitions;


		BBState& OnUpdate(OnUpdateState InOnUpdate)
		{
			OnUpdateDelegate = InOnUpdate;
			return *this;
		}
		BBState& OnEnter(OnEnterStateDelegate InOnEnter)
		{
			OnEnterDelegate = InOnEnter;
			return *this;
		}
		BBState& OnLeave(OnLeaveStateDelegate InOnLeave)
		{
			OnLeaveDelegate = InOnLeave;
			return *this;
		}
		BBState& Transition(TStateType TargetState, TEventType TargetEvent, Conditional CanEnter = Conditional())
		{
			Transitions.Add(BBTransition(TargetState, TargetEvent, CanEnter));
			return *this;
		}
		// Allows re-entering the same state with a full leave/enter cycle
		// This is the same as writing State(MyState).Transition(MyState, TargetEvent, Conditional)
		// but in a more convenient way
		BBState& ReEnter(TEventType TargetEvent, Conditional CanEnter = Conditional())
		{
			Transitions.Add(BBTransition(State, TargetEvent, CanEnter));
			return *this;
		}
		BBState& IgnoreEvent(TEventType Event)
		{
			IgnoredEvents.Add(Event);
			return *this;
		}
		BBState& IgnoreNotSetEvents(bool Ignore = true)
		{
			IgnoresMissingTransitions = Ignore;
			return *this;
		}
		// Triggers callback for this event set in the current state, but does not trigger any transition.
		// Overwrites previously set handlers for this event
		BBState& Handle(TEventType Event, OnHandleEventDelegate Handler)
		{
			EventHandlers.Add(Event, Handler);
			return *this;
		}

	private:
		bool IgnoresEvent(TEventType Event)
		{
			return IgnoresMissingTransitions || IgnoredEvents.Contains(Event);
		}
		void Leave(TStateType NewState, TEventType TriggeringEvent)
		{
			OnLeaveDelegate.ExecuteIfBound(NewState, TriggeringEvent);
		}
		void Enter(TStateType OldState, TEventType TriggeringEvent)
		{
			OnEnterDelegate.ExecuteIfBound(OldState, TriggeringEvent);
		}

		BBTransition* ProcessEvent(TEventType Event, bool& HadMatchingHandlers)
		{
			for (int32 TransitionIndex = 0; TransitionIndex < Transitions.Num(); ++TransitionIndex)
			{
				BBTransition* CurrentTransition = &Transitions[TransitionIndex];
				if (CurrentTransition->TriggeringEvent == Event)
				{
					HadMatchingHandlers = true;
				}
				if (CurrentTransition->ProcessEvent(Event))
				{
					return CurrentTransition;
				}
			}
			OnHandleEventDelegate* Delegate = EventHandlers.Find(Event);
			if (Delegate != nullptr)
			{
				HadMatchingHandlers = true;
				Delegate->ExecuteIfBound(Event);
			}
			return nullptr;
		}

		void Update(float ElapsedTime)
		{
			OnUpdateDelegate.ExecuteIfBound(ElapsedTime);
		}

		friend class TBBStateMachine<TStateType, TEventType>;
	};

	BBState& State(TStateType InState)
	{
		BBState NewState(InState);
		BBState* UsedState = States.Find(NewState);
		if (UsedState == nullptr)
		{
			bool AlreadyAdded = false;
			auto ElementID = States.Add(NewState, &AlreadyAdded);
			ensureMsgf(!AlreadyAdded, TEXT("State was already added but we could not find it previously"));
			UsedState = &States[ElementID];
		}
		ensure(UsedState);
		return *UsedState;
	}

	TStateType GetCurrentState() const
	{
		return CurrentState;
	}

	void Update(float DeltaSeconds)
	{
		if (InitialStatesNeedsEnter)
		{
			UE_LOG(LogStateMachine, Verbose, TEXT(">>> Entering State '%s' due to event '%s'"),
				   *GetStateString(CurrentState),
				   *GetEventString(InitialEvent)
			);
			State(CurrentState).Enter(CurrentState, InitialEvent);
			InitialStatesNeedsEnter = false;
		}
		TEventType PendingEvent;
		while (PendingEvents.Dequeue(PendingEvent))
		{
			ProcessEvent(PendingEvent);
		}
		State(CurrentState).Update(DeltaSeconds);
	}

	void NotifyLatent(TEventType Event)
	{
		bool enqueued = PendingEvents.Enqueue(Event);
		ensure(enqueued);
	}

	void NotifyImmediate(TEventType Event)
	{
		ProcessEvent(Event);
	}


	TSharedPtr<IBBStringHelper<TEventType>> EventStringifier;
	TSharedPtr<IBBStringHelper<TStateType>> StateStringifier;

	// As TEventType and TStateType could be anything we need something that can convert it to strings
	void SetLogStringifier(
		TSharedRef<IBBStringHelper<TStateType>> InStateStringifer,
		TSharedRef<IBBStringHelper<TEventType>> InEventStringifer)
	{
		EventStringifier = InEventStringifer;
		StateStringifier = InStateStringifer;
	}

private:

	inline FString GetStateString(TStateType State)
	{
		if (StateStringifier.IsValid())
		{
			return StateStringifier->GetDisplayString(State);
		}
		else
		{
			return TEXT("<NO STRINGIFIER>");
		}
	}
	inline FString GetEventString(TEventType Event)
	{
		if (EventStringifier.IsValid())
		{
			return EventStringifier->GetDisplayString(Event);
		}
		else
		{
			return TEXT("<NO STRINGIFIER>");
		}
	}

	template<typename ElementType, bool bInAllowDuplicateKeys = false>
	struct StateKeyFuncs : BaseKeyFuncs<ElementType, ElementType, bInAllowDuplicateKeys>
	{
		typedef typename TCallTraits<ElementType>::ParamType KeyInitType;
		typedef typename TCallTraits<ElementType>::ParamType ElementInitType;

		/**
		* @return The key used to index the given element.
		*/
		static FORCEINLINE KeyInitType GetSetKey(ElementInitType Element)
		{
			return Element;
		}

		/**
		* @return True if the states match.
		*/
		static FORCEINLINE bool Matches(KeyInitType A, KeyInitType B)
		{
			return A.State == B.State;
		}

		/** Return the state as hash */
		static FORCEINLINE uint32 GetKeyHash(KeyInitType Key)
		{
			return static_cast<uint32>(Key.State);
		}
	};

	TQueue<TEventType> PendingEvents;
	TSet<BBState, StateKeyFuncs<BBState>> States;
	TStateType CurrentState;
	bool InitialStatesNeedsEnter = true;
	TEventType InitialEvent;

	void ProcessEvent(TEventType Event)
	{
		BBTransition* TransitionToPerform = nullptr;
		bool HadMatchingTransitions = false;
		TransitionToPerform = State(CurrentState).ProcessEvent(Event, HadMatchingTransitions);
		const bool IgnoredEvent = State(CurrentState).IgnoresEvent(Event);
		if (TransitionToPerform != nullptr)
		{
			UE_LOG(LogStateMachine, Verbose, TEXT("<<< Leaving State '%s' due to event '%s'"),
				   *GetStateString(CurrentState),
				   *GetEventString(Event)
			);
			State(CurrentState).Leave(TransitionToPerform->TargetState, Event);
			TStateType OldState = CurrentState;
			CurrentState = TransitionToPerform->TargetState;
			UE_LOG(LogStateMachine, Verbose, TEXT(">>> Entering State '%s' due to event '%s'"),
				   *GetStateString(CurrentState),
				   *GetEventString(Event)
			);
			State(CurrentState).Enter(OldState, Event);
		}
		else if (!HadMatchingTransitions && !IgnoredEvent)
		{
			// This is for self checking that we did not forget some transition
			ensureMsgf(
				false,
				TEXT("Event '%s' was not handled by state '%s' but also not set to be ignored"),
				*GetEventString(Event),
				*GetStateString(CurrentState));
		}
		else if (IgnoredEvent)
		{
			UE_LOG(LogStateMachine, VeryVerbose, TEXT(">>> State '%s' ignored event '%s'"),
				   *GetStateString(CurrentState),
				   *GetEventString(Event)
			);
		}
		else if (HadMatchingTransitions)
		{
			UE_LOG(LogStateMachine, Verbose, TEXT(">>> State '%s' handled event '%s'"),
				   *GetStateString(CurrentState),
				   *GetEventString(Event)
			);
		}
	}
};
