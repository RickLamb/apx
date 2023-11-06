
//  Copyright (c) Rick Lamb
//  SPDX-License-Identifier: CC-BY-NC-ND-4.0

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


//===========================================================================
//  Visitor to help component meta class construction
//===========================================================================

#ifndef __CPP2_COMPONENT
#define __CPP2_COMPONENT

#include "parse.h"


namespace cpp2 {

//-----------------------------------------------------------------------
//
//  Visitors for processing component class
//
//-----------------------------------------------------------------------
//
	

//  parse_params takes a meta::function_declaration and parses out the parameters
class parse_params
{
public:
	struct Param
	{
		enum class Direction { in, out, inout };
		enum class Type { Unknown, This, Ctx, MyComponent, AllComponents };
		Direction		m_dir;
		Type			m_type;
		std::string		m_typeName;
		std::string		m_name;
	};

	bool								m_started;
	bool								m_ended;
	bool								m_error;
	std::string							m_errorMsg;

	int									m_declarationDepth;
	bool								m_parsingParam;
	bool								m_parsingParamType;
	bool								m_parsingParamTypeTemplateArgs;
	Param								m_current;

	std::string							m_functionId;
	std::vector< Param >				m_params;

	parse_params()
	: m_started(false)
	, m_ended(false)
	, m_parsingParam(false)
	, m_parsingParamType(false)
	, m_parsingParamTypeTemplateArgs(false)
	, m_error(false)
	, m_declarationDepth(0)
	, m_functionId("")
	{
	}

	auto start(token const& n, int indent) -> void
	{
		auto t = n.type();
		std::string txt = n.to_string(true);

		if (m_started) {
			if (m_parsingParam) {
				if (m_parsingParamType) {
					if (m_parsingParamTypeTemplateArgs) {
						m_current.m_typeName = txt;
						//TODO: check template name
						m_current.m_type = Param::Type::AllComponents;
					}
					else {
						m_current.m_typeName = txt;
						if (txt == "ECS_Context") {
							m_current.m_type = Param::Type::Ctx;
						} else if (txt == "_") {
							if (m_current.m_name == "this") {
								m_current.m_type = Param::Type::This;
							}
							// otherwise this will be an error eventually
						} else {
							m_current.m_type = Param::Type::MyComponent;
						}
					}
				}
				else {
					m_current.m_name = txt;
				}
			}
			else if (m_declarationDepth == 1) {
				if (m_functionId == "") {
					m_functionId = txt;
				}
				// otherwise could be in function body... maybe handle that with flag...
			}
		}

		//o << pre(indent) << n.to_string() << "\n";
	}

	auto start(literal_node const&, int indent) -> void
	{
		//o << pre(indent) << "literal" << "\n";
	}

	auto start(expression_node const& n, int indent) -> void
	{
		//o << pre(indent) << "expression - "
		//	<< n.num_subexpressions << " subexpressions, my_statement ["
		//	<< static_cast<void const*>(n.my_statement) << "]\n";
	}

	auto start(expression_list_node::term const& n, int indent) -> void
	{
		//o << pre(indent) << "expression-list term\n";
		//if (n.pass == passing_style::out) {
		//	o << pre(indent + 1) << "out\n";
		//}
	}

	auto start(expression_list_node const&, int indent) -> void
	{
		//o << pre(indent) << "expression-list\n";
	}

	auto start(primary_expression_node const&, int indent) -> void
	{
		//o << pre(indent) << "primary-expression\n";
	}

	auto start(prefix_expression_node const&, int indent) -> void
	{
		//o << pre(indent) << "prefix-expression\n";
	}

	auto start(is_as_expression_node const&, int indent) -> void
	{
		//o << pre(indent) << "is-as-expression\n";
	}

	template<String Name, typename Term>
	auto start(binary_expression_node<Name, Term> const&, int indent) -> void
	{
		//o << pre(indent) << Name.value << "-expression\n";
	}

	auto start(expression_statement_node const& n, int indent) -> void
	{
		//o << pre(indent) << "expression-statement - [" << static_cast<void const*>(&n) << "]\n";
	}

	auto start(postfix_expression_node const&, int indent) -> void
	{
		//o << pre(indent) << "postfix-expression\n";
	}

	auto start(unqualified_id_node const&, int indent) -> void
	{
		//o << pre(indent) << "unqualified-id\n";
	}

	auto start(qualified_id_node const&, int indent) -> void
	{
		//o << pre(indent) << "qualified-id\n";
	}

	auto start(type_id_node const&, int indent) -> void
	{
		if (m_parsingParam) {
			m_parsingParamType = true;
		}
	}

	auto end(type_id_node const&, int indent) -> void
	{
		if (m_parsingParam) {
			m_parsingParamType = false;
		}
	}


	auto start(id_expression_node const&, int indent) -> void
	{
		//o << pre(indent) << "id-expression\n";
	}

	auto start(statement_node const&, int indent) -> void
	{
		//o << pre(indent) << "statement\n";
	}

	auto start(compound_statement_node const&, int indent) -> void
	{
		//o << pre(indent) << "compound-statement\n";
	}

	auto start(selection_statement_node const& n, int indent) -> void
	{
		//o << pre(indent) << "selection-statement\n";
		//o << pre(indent + 1) << "is_constexpr: " << __as<std::string>(n.is_constexpr) << "\n";
	}

	auto start(alternative_node const&, int indent) -> void
	{
		//o << pre(indent) << "alternative\n";
	}

	auto start(jump_statement_node const&, int indent) -> void
	{
		//o << pre(indent) << "jump\n";
	}

	auto start(inspect_expression_node const& n, int indent) -> void
	{
		//o << pre(indent) << "inspect-expression\n";
		//o << pre(indent + 1) << "is_constexpr: " << __as<std::string>(n.is_constexpr) << "\n";
	}

	auto start(return_statement_node const&, int indent) -> void
	{
		//o << pre(indent) << "return-statement\n";
	}

	auto start(iteration_statement_node const& n, int indent) -> void
	{
		//o << pre(indent) << "iteration-statement\n";
		//assert(n.identifier);
		//o << pre(indent + 1) << "identifier: " << std::string_view(*n.identifier) << "\n";

	}

	auto start(contract_node const& n, int indent) -> void
	{
		//o << pre(indent) << "contract\n";
		//assert(n.kind);
		//o << pre(indent + 1) << "kind: " << std::string_view(*n.kind) << "\n";
		//if (n.message) {
		//	o << pre(indent + 1) << "message: " << std::string_view(*n.message) << "\n";
		//}
		//if (!n.captures.members.empty()) {
		//	o << pre(indent + 1) << "captures: " << n.captures.members.size() << "\n";
		//}
	}

	auto start(type_node const&, int indent) -> void
	{
		//o << pre(indent) << "user-defined type\n";
	}

	auto start(namespace_node const&, int indent) -> void
	{
		//o << pre(indent) << "namespace\n";
	}

	auto start(function_type_node const& n, int indent) -> void
	{
		//o << pre(indent) << "function\n";
		//o << pre(indent + 1) << "throws: " << __as<std::string>(n.throws) << "\n";
		//if (n.returns.index() == function_type_node::id) {
		//	auto& r = std::get<function_type_node::id>(n.returns);
		//	if (r.pass != passing_style::invalid) {
		//		o << pre(indent + 1) << "returns by: " << to_string_view(r.pass) << "\n";
		//	}
		//}
	}

	auto start(function_returns_tag const&, int indent) -> void
	{
		//o << pre(indent) << "function returns\n";
	}

	auto start(alias_node const& n, int indent) -> void
	{
		//o << pre(indent) << "alias\n";
		//switch (n.initializer.index()) {
		//break; case alias_node::a_type:
		//	o << pre(indent + 1) << "type\n";
		//break; case alias_node::a_namespace:
		//	o << pre(indent + 1) << "namespace\n";
		//break; case alias_node::an_object:
		//	o << pre(indent + 1) << "object\n";
		//break; default:
		//	o << pre(indent + 1) << "ICE - invalid variant state\n";
		//}
	}

	auto start(declaration_node const& n, int indent) -> void
	{
		if (!m_started) {
			m_started = true;
			m_declarationDepth = 0;
		}
		m_declarationDepth++;

		if (m_declarationDepth == 1) {
			if (n.type.index() == declaration_node::a_function) {

			} else {
				m_error = true;
				m_errorMsg = "Passed declaration_node is not a function";
			}
		}
	}

	auto end(declaration_node const& n, int) -> void
	{
		m_declarationDepth--;
		if (m_declarationDepth == 0) {
			m_ended = true;
		}
		if (m_declarationDepth < 0) {
			//don't think this can happen
			m_error = true;
			m_errorMsg = "Mismatched declaration node start-end";
		}
	}

	auto start(parameter_declaration_node const& n, int indent) -> void
	{
		if (m_parsingParam) {
			m_error = true; m_errorMsg = "Nested parameter_declaration_node";
		}

		m_parsingParam = true;

		//TODO: not clear we cover all paths here to init this to something real
		m_current.m_type = Param::Type::Unknown;

		//o << pre(indent + 1);
		switch (n.pass) {
		break; case passing_style::in: m_current.m_dir = Param::Direction::in;
		break; case passing_style::inout: m_current.m_dir = Param::Direction::inout;
		break; case passing_style::out: m_current.m_dir = Param::Direction::out;
		break; case passing_style::copy: m_error = true; m_errorMsg = "Passing style copy not allowed";
		break; case passing_style::move: m_error = true; m_errorMsg = "Passing style move not allowed";
		break; case passing_style::forward: m_error = true; m_errorMsg = "Passing style forward not allowed";
		break; default:;
		}

		assert(n.declaration);
	}

	auto end(parameter_declaration_node const& n, int indent) -> void
	{
		m_parsingParam = false;
		m_params.push_back(m_current);
	}

	auto start(parameter_declaration_list_node const&, int indent) -> void
	{
		//o << pre(indent) << "parameter-declaration-list\n";
	}

	auto start(template_args_tag const&, int indent) -> void
	{
		if (m_parsingParam && m_parsingParamType) {
			m_parsingParamTypeTemplateArgs = true;
		}
	}
	auto end(template_args_tag const&, int indent) -> void
	{
		m_parsingParamTypeTemplateArgs = false;
	}


	auto start(translation_unit_node const&, int indent) -> void
	{
		//o << pre(indent) << "translation-unit\n";
	}

	auto start(auto const&, int indent) -> void
	{
		//o << pre(indent) << "UNRECOGNIZED -- FIXME\n";
	}

	auto end(auto const&, int) -> void
	{
		//  Ignore other node types
	}
};


class parse_tree_xml
{
public:
	std::ostream& o;

	parse_tree_xml(std::ostream& out) : o{ out } { }

	//-----------------------------------------------------------------------
	//  pre: Get an indentation prefix
	//
	inline static int         indent_spaces = 2;
	inline static std::string indent_str = std::string(1024, ' ');    // "1K should be enough for everyone"

	auto pre(int indent)
		-> std::string_view
	{
		assert(indent >= 0);
		return {
			indent_str.c_str(),
			as<size_t>(std::min(indent * indent_spaces, __as<int>(std::ssize(indent_str))))
		};
	}

	auto start(token const& n, int indent) -> void
	{
		o << pre(indent) << "token - " << n.to_string() << "\n";
	}

	auto start(literal_node const&, int indent) -> void
	{
		o << pre(indent) << "literal" << "\n";
	}

	auto start(expression_node const& n, int indent) -> void
	{
		o << pre(indent) << "expression - "
			<< n.num_subexpressions << " subexpressions, my_statement ["
			<< static_cast<void const*>(n.my_statement) << "]\n";
	}

	auto start(expression_list_node::term const& n, int indent) -> void
	{
		o << pre(indent) << "expression-list term\n";
		if (n.pass == passing_style::out) {
			o << pre(indent + 1) << "out\n";
		}
	}

	auto start(expression_list_node const&, int indent) -> void
	{
		o << pre(indent) << "expression-list\n";
	}

	auto start(primary_expression_node const&, int indent) -> void
	{
		o << pre(indent) << "primary-expression\n";
	}

	auto start(prefix_expression_node const&, int indent) -> void
	{
		o << pre(indent) << "prefix-expression\n";
	}

	auto start(is_as_expression_node const&, int indent) -> void
	{
		o << pre(indent) << "is-as-expression\n";
	}

	template<String Name, typename Term>
	auto start(binary_expression_node<Name, Term> const&, int indent) -> void
	{
		o << pre(indent) << Name.value << "-expression\n";
	}

	auto start(expression_statement_node const& n, int indent) -> void
	{
		o << pre(indent) << "expression-statement - [" << static_cast<void const*>(&n) << "]\n";
	}

	auto start(postfix_expression_node const&, int indent) -> void
	{
		o << pre(indent) << "postfix-expression\n";
	}

	auto start(unqualified_id_node const&, int indent) -> void
	{
		o << pre(indent) << "unqualified-id\n";
	}

	auto start(qualified_id_node const&, int indent) -> void
	{
		o << pre(indent) << "qualified-id\n";
	}

	auto start(type_id_node const&, int indent) -> void
	{
		o << pre(indent) << "type-id\n";
	}

	auto start(id_expression_node const&, int indent) -> void
	{
		o << pre(indent) << "id-expression\n";
	}

	auto start(statement_node const&, int indent) -> void
	{
		o << pre(indent) << "statement\n";
	}

	auto start(compound_statement_node const&, int indent) -> void
	{
		o << pre(indent) << "compound-statement\n";
	}

	auto start(selection_statement_node const& n, int indent) -> void
	{
		o << pre(indent) << "selection-statement\n";
		o << pre(indent + 1) << "is_constexpr: " << __as<std::string>(n.is_constexpr) << "\n";
	}

	auto start(alternative_node const&, int indent) -> void
	{
		o << pre(indent) << "alternative\n";
	}

	auto start(jump_statement_node const&, int indent) -> void
	{
		o << pre(indent) << "jump\n";
	}

	auto start(inspect_expression_node const& n, int indent) -> void
	{
		o << pre(indent) << "inspect-expression\n";
		o << pre(indent + 1) << "is_constexpr: " << __as<std::string>(n.is_constexpr) << "\n";
	}

	auto start(return_statement_node const&, int indent) -> void
	{
		o << pre(indent) << "return-statement\n";
	}

	auto start(iteration_statement_node const& n, int indent) -> void
	{
		o << pre(indent) << "iteration-statement\n";
		assert(n.identifier);
		o << pre(indent + 1) << "identifier: " << std::string_view(*n.identifier) << "\n";
	}

	auto start(contract_node const& n, int indent) -> void
	{
		o << pre(indent) << "contract\n";
		assert(n.kind);
		o << pre(indent + 1) << "kind: " << std::string_view(*n.kind) << "\n";
		if (n.message) {
			o << pre(indent + 1) << "message: " << std::string_view(*n.message) << "\n";
		}
		if (!n.captures.members.empty()) {
			o << pre(indent + 1) << "captures: " << n.captures.members.size() << "\n";
		}
	}

	auto start(type_node const&, int indent) -> void
	{
		o << pre(indent) << "user-defined type\n";
	}

	auto start(namespace_node const&, int indent) -> void
	{
		o << pre(indent) << "namespace\n";
	}

	auto start(function_type_node const& n, int indent) -> void
	{
		o << pre(indent) << "function\n";
		o << pre(indent + 1) << "throws: " << __as<std::string>(n.throws) << "\n";
		if (n.returns.index() == function_type_node::id) {
			auto& r = std::get<function_type_node::id>(n.returns);
			if (r.pass != passing_style::invalid) {
				o << pre(indent + 1) << "returns by: " << to_string_view(r.pass) << "\n";
			}
		}
	}

	auto start(function_returns_tag const&, int indent) -> void
	{
		o << pre(indent) << "function returns\n";
	}

	auto start(template_args_tag const&, int indent) -> void
	{
		o << pre(indent) << "template arguments\n";
	}

	auto start(alias_node const& n, int indent) -> void
	{
		o << pre(indent) << "alias\n";
		switch (n.initializer.index()) {
		break; case alias_node::a_type:
			o << pre(indent + 1) << "type\n";
		break; case alias_node::a_namespace:
			o << pre(indent + 1) << "namespace\n";
		break; case alias_node::an_object:
			o << pre(indent + 1) << "object\n";
		break; default:
			o << pre(indent + 1) << "ICE - invalid variant state\n";
		}
	}

	auto start(declaration_node const& n, int indent) -> void
	{
		o << pre(indent) << "declaration [" << &n << "]\n";
		o << pre(indent + 1) << "parent: [" << n.parent_declaration << "]\n";
		switch (n.type.index()) {
		break; case declaration_node::a_function:
			o << pre(indent + 1) << "function\n";
		break; case declaration_node::an_object:
			o << pre(indent + 1) << "object\n";
		break; case declaration_node::a_type:
			o << pre(indent + 1) << "type\n";
		break; case declaration_node::a_namespace:
			o << pre(indent + 1) << "namespace\n";
		break; case declaration_node::an_alias:
			o << pre(indent + 1) << "alias\n";
		break; default:
			o << pre(indent + 1) << "ICE - invalid variant state\n";
		}
		if (!n.is_default_access()) {
			o << pre(indent + 1) << "access: " << to_string(n.access) << "\n";
		}
		if (!n.captures.members.empty()) {
			o << pre(indent + 1) << "captures: " << n.captures.members.size() << "\n";
		}
	}

	auto start(parameter_declaration_node const& n, int indent) -> void
	{
		o << pre(indent) << "parameter-declaration\n";

		o << pre(indent + 1);
		switch (n.pass) {
		break; case passing_style::in: o << "in";
		break; case passing_style::copy: o << "copy";
		break; case passing_style::inout: o << "inout";
		break; case passing_style::out: o << "out";
		break; case passing_style::move: o << "move";
		break; case passing_style::forward: o << "forward";
		break; default:;
		}

		o << pre(indent + 1);
		switch (n.mod) {
		break; case parameter_declaration_node::modifier::implicit: o << "implicit";
		break; case parameter_declaration_node::modifier::virtual_: o << "virtual";
		break; case parameter_declaration_node::modifier::override_: o << "override";
		break; case parameter_declaration_node::modifier::final_: o << "final";
		break; default:;
		}
		o << "\n";

		assert(n.declaration);
	}

	auto start(parameter_declaration_list_node const&, int indent) -> void
	{
		o << pre(indent) << "parameter-declaration-list\n";
	}

	auto start(translation_unit_node const&, int indent) -> void
	{
		o << pre(indent) << "translation-unit\n";
	}

	auto start(auto const&, int indent) -> void
	{
		o << pre(indent) << "UNRECOGNIZED -- FIXME\n";
	}

	auto end(auto const&, int) -> void
	{
		//  Ignore other node types
	}
};



}

#endif
