#include "pch.h"
#if 1
#include "movement.h"

namespace movement
{	
	static void Cmd_Test()
	{
		const char* jump_slowdownEnable = stock::Info_ValueForKey(cs1, "jump_slowdownEnable");
		printf("############# jump_slowdownEnable: %s\n", jump_slowdownEnable);
	}

	/*void Jump_ApplySlowdown()
	{
		stock::playerState_t* ps = (*stock::pm)->ps;

		if (ps->pm_flags & PMF_JUMPING)
		{
			float scale = 1.0f;

			if (ps->pm_time <= JUMP_LAND_SLOWDOWN_TIME)
			{
				if (!ps->pm_time)
				{
					if ((float)(ps->jumpOriginZ + 18.0) <= ps->origin[2])
					{
						ps->pm_time = 1200;
						scale = 0.5f;
					}
					else
					{
						ps->pm_time = JUMP_LAND_SLOWDOWN_TIME;
						scale = 0.64999998f;
					}
				}
			}
			else
			{
				// Clear jump state
				ps->pm_flags &= ~PMF_JUMPING;
				ps->jumpOriginZ = 0.0;
				scale = 0.64999998f;
			}

			const char* jump_slowdownEnable = stock::Info_ValueForKey(cs1, "jump_slowdownEnable");
			if (*jump_slowdownEnable && atoi(jump_slowdownEnable) == 0)
				return;
			VectorScale(ps->velocity, scale, ps->velocity);
		}
	}
	uintptr_t resume_addr_PM_WalkMove;
	// resume address: 3000928b
	// hook address: 300091f1
	__declspec(naked) void hook_PM_WalkMove_Naked()
	{
		__asm
		{
			pushad;
			call Jump_ApplySlowdown;
			popad;
			jmp resume_addr_PM_WalkMove;
		}
	}//*/

	/*void hook_PM_SlideMove(float primal_velocity_0, float primal_velocity_1, float primal_velocity_2)
	{
		const char* jump_slowdownEnable = stock::Info_ValueForKey(cs1, "jump_slowdownEnable");
		if (*jump_slowdownEnable && atoi(jump_slowdownEnable) == 0)
			return;
		
		stock::playerState_t* ps = (*stock::pm)->ps;
		if (ps->pm_time)
		{
			ps->velocity[0] = primal_velocity_0;
			ps->velocity[1] = primal_velocity_1;
			ps->velocity[2] = primal_velocity_2;
		}
	}
	uintptr_t resume_addr_PM_SlideMove;
	__declspec(naked) void hook_PM_SlideMove_Naked()
	{
		__asm
		{
			mov eax, dword ptr[esp + 0x110 - 0xA8]
			mov ecx, dword ptr[esp + 0x110 - 0xAC]
			mov edx, dword ptr[esp + 0x110 - 0xB0]

			push eax
			push ecx
			push edx

			call hook_PM_SlideMove
			add esp, 12

			jmp resume_addr_PM_SlideMove
		}
	}//*/

	void Jump_GetLandFactor()
	{
		//int* pm = (int*)(cgame_mp + 0x1a0ed0);
		stock::playerState_t* ps = (*stock::pm)->ps;

		double factor;

		const char* jump_slowdownEnable = stock::Info_ValueForKey(cs1, "jump_slowdownEnable");
		if (*jump_slowdownEnable && atoi(jump_slowdownEnable) == 0)
			factor = 1.0;
		else if (ps->pm_time < 1700)
			factor = (double)ps->pm_time * 0.00088235294 + 1.0;
		else
			factor = 2.5;

		__asm fld factor
	}
	uintptr_t resume_addr_Jump_Start;
	//resume_addr_Jump_Start = CGAME_OFF(0x30009539); // 30008cda
	// 30009521
	// hook address: 30008cb8
	__declspec(naked) void hook_Jump_Start_Naked()
	{
		__asm
		{
			pushad;
			call Jump_GetLandFactor;
			popad;
			jmp resume_addr_Jump_Start; // 30008cda
		}
	}//*/
	
	void custom_PM_GetReducedFriction()
	{
		double friction;

		const char* jump_slowdownEnable = stock::Info_ValueForKey(cs1, "jump_slowdownEnable");
		if (*jump_slowdownEnable && atoi(jump_slowdownEnable) == 0)
		{
			friction = 1.0;
			//printf("Jump slowdown disabled\n");
		}
		else
		{
			//int* pm = (int*)(cgame_mp + 0x1a0ed0);
			stock::playerState_t* ps = (*stock::pm)->ps;

			if (ps->pm_time < 1700)
				friction = (double)ps->pm_time * 0.00088235294 + 1.0;
			else
				friction = 2.5;
		}
		//printf("friction: %f\n", friction);//*/

		__asm fld friction
	}

	// 30008410
	__declspec(naked) void custom_PM_GetReducedFriction_Naked()
	{
		__asm
		{
			pushad
			call custom_PM_GetReducedFriction
			popad
			ret
		}
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{	
			stock::Cmd_AddCommand("test", Cmd_Test);
		}

		void post_cgame() override
		{
			//utils::hook::jump(ABSOLUTE_CGAME_MP(0x30032fe8), stub_cg_zoomSensitivity_calculation);
			//utils::hook::jump(ABSOLUTE_CGAME_MP(0x3000d82b), stub_PM_StepSlideMove_PM_ClipVelocity);
			
			/*
			Some Experiments
			
			utils::hook::jump(ABSOLUTE_CGAME_MP(0x30008410), custom_PM_GetReducedFriction_Naked);
			utils::hook::jump(ABSOLUTE_CGAME_MP(0x30008c70), custom_PM_GetReducedFriction_Naked);
			utils::hook::jump(ABSOLUTE_CGAME_MP(0x30008bd3), custom_PM_GetReducedFriction_Naked);
			
			hook address: 3000f150
			resume address: 3000f16d
			*/
			//resume_addr_PM_WalkMove = ABSOLUTE_CGAME_MP(0x3000928b); // 3000928b
			//utils::hook::jump(ABSOLUTE_CGAME_MP(0x300091f1), hook_PM_WalkMove_Naked);

			//resume_addr_PM_SlideMove = ABSOLUTE_CGAME_MP(0x3000f16d); // 3000928b
			//utils::hook::jump(ABSOLUTE_CGAME_MP(0x3000f150), hook_PM_SlideMove_Naked);
			
			resume_addr_Jump_Start = ABSOLUTE_CGAME_MP(0x30008cda); // 30008cda
			utils::hook::jump(ABSOLUTE_CGAME_MP(0x30008cb8), hook_Jump_Start_Naked);

			utils::hook::jump(ABSOLUTE_CGAME_MP(0x30008410), custom_PM_GetReducedFriction_Naked);
		}
	};
}

REGISTER_COMPONENT(movement::component)
#endif