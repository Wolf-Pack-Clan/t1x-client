#include "pch.h"

#include "component_loader.h"

void component_loader::register_component(std::unique_ptr<component_interface>&& component)
{
	get_components().push_back(std::move(component));
}

bool component_loader::post_start()
{
	static auto handled = false;
	if (handled) return true;
	handled = true;

	try
	{
		for (const auto& component : get_components())
			component->post_start();
	}
	catch (premature_shutdown_trigger&)
	{
		return false;
	}

	return true;
}

bool component_loader::post_load()
{
	static auto handled = false;
	if (handled) return true;
	handled = true;

	try
	{
		for (const auto& component : get_components())
			component->post_load();
	}
	catch (premature_shutdown_trigger&)
	{
		return false;
	}

	return true;
}

void component_loader::post_unpack()
{
	static auto handled = false;
	if (handled) return;
	handled = true;

	for (const auto& component : get_components())
		component->post_unpack();
}

void component_loader::post_cgame()
{
	for (const auto& component : get_components())
		component->post_cgame();
}
void component_loader::post_ui_mp()
{
	for (const auto& component : get_components())
		component->post_ui_mp();
}

void component_loader::pre_destroy()
{
	static auto handled = false;
	if (handled) return;
	handled = true;

	for (const auto& component : get_components())
		component->pre_destroy();
}

void* component_loader::load_import(const std::string& library, const std::string& function)
{
	void* function_ptr = nullptr;

	for (const auto& component : get_components())
	{
		auto* const component_function_ptr = component->load_import(library, function);
		if (component_function_ptr)
			function_ptr = component_function_ptr;
	}

	return function_ptr;
}

void component_loader::trigger_premature_shutdown()
{
	throw premature_shutdown_trigger();
}

std::vector<std::unique_ptr<component_interface>>& component_loader::get_components()
{
	using component_vector = std::vector<std::unique_ptr<component_interface>>;
	using component_vector_container = std::unique_ptr<component_vector, std::function<void(component_vector*)>>;

	static component_vector_container components(new component_vector, [](component_vector* component_vector)
		{
			pre_destroy();
			delete component_vector;
		});

	return *components;
}