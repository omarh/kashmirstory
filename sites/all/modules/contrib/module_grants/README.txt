$Id: README.txt,v 1.1 2009/03/20 01:26:18 rdeboer Exp $

DESCRIPTION
===========
This module gets around two quirks in the 6.x core Node module. 
Currently the Node module:
- causes access grants to be ignored for unpublished content;
- ORs together access grants coming from multiple modules; this results
  in content being made accessible by one module when access had already
  been restricted by another, which is undesirable in most cases.

The module ensures that access grants are tested for unpublished content just 
as they are for published content, so that using the Workflow module (or any 
other module that uses the node_access table) you can implement workflows that 
deal effectively with content moving from author via moderator to publisher 
BEFORE it is published (which is where it's needed most, once content is 
visible for all to see, it's a bit late to start a publication workflow 
process!).
Using Taxonomy Access Control (or Lite) you can restrict access to content
to user-defined "vocabularies" such as departments or regions. With Module
Grants this will work for unpublished content just as it does for published 
content.
Moreover when Workflow and TAC or (TAC-Lite) are used together this module
makes sure that the combination exhibits the expected behaviour: access is
granted to content only when it is in the correct state AND of the appropriate
vocabulary "term" (such as department, country, etc.).
The module_grants module achieves this by AND-ing rather than OR-ing the grants.

After installation users will have access to a new menu item, "My content" that 
shows a list of all content accessible to them based on their permissions and 
access grants as determined by enabled modules implementing hook_node_grants().

INSTALLATION
============
1. Place the "module_grants" folder in your "sites/all/modules" directory.
2. Enable the module under Administer >> Site building >> Modules.
3. Under Administer >> User management >> Permissions, section "module_grants
   module" tick the "access content summary" box for "authenticated user".
   There's usually no need to tick "administer nodes", which is good because
   "administer nodes" equates to almost god-like powers that you wouldn't 
   normally give to normal users.
4. If required, install and enable as many modules for content access control 
   as you need for your situation. Typical examples are Taxonomy Access Control
   (or use TAC Lite) and Workflow.

USAGE
=====
The module creates a new root menu item, 'My content' visible to the
administrator and to roles that have the "access content summary" permission. 
These users will see two new tabs:
 o "Viewable" shows all nodes that may be viewed by the logged-in user as
   granted by enabled modules that implement hook_node_grants()
 o "Editable" shows all nodes that may be edited by the logged-in user
 
You can use this module in combination with TAC or TAC-Lite for fine-grained
access control based on vocabularies (such as "department") assigned to the 
various content types. You can then create department-specific roles (eg 
Sports Author, Music Author) and enforce that these roles can only access
content belonging to their departments, whether it's published or not.
Create your grants "schemes" on this page: Administer >> User management >> 
Access control by taxonmy.

In addition you may want to install the Workflow module to further segragate
roles (eg author and moderator) via access control based on states such as
"in draft", "in review" and "published". See Administer >> Site building >>
Workflow.

The module makes sure that access to content is given only when both the 
TAC (Lite) and the Workflow Access modules grant it (as opposed to one OR
the other).

Be aware that any permissions given in the "node module" section override the
access grants given by the Workflow and TAC-Lite modules, so you probably only 
want to assign a few creation permissions in the node module and grant 
view, update and delete via TAC/TAC-Lite and/or Workflow.

This module also works well with the Revisioning module for creating effective
publication workflows operating on published as well as unpublished content
revisions. 

AUTHOR
======
Rik de Boer, IBS, Melbourne, Australia.
