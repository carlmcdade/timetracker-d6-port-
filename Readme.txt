Timetacker for Drupal 6 and Open Atrium D6
========================

This is Drupal module designed to clone functionality found at Toggl.com into Drupal.

The following instructions are for the latest version of timetracker and are subject 
to change.


Installation:


Activate module:

Module activation is standard and creates the necessary database tables.

Create menu link:

Add a link to the features menu with the path of "timetracker/timetracker/users"
This link will automatically have an icon set. The icon can be changed via the css file.


Set permissions:

As of now the timetracker is a free for all. There is a backend administration section and
frontend UI. But there are no granular permissions set for operations like viewing reports,
changing information etc. This is done on purpose as the use case is for open information between
factions. If you want a more definative or custom user access structure then contact the developer.

Set vocabulary:

Time tracker adminstration settings allow the setting of the vocabulary to be used as container
for all projects(terms)

Create groups:

Projects can be given a group designation for bookeeping purposes. Setting a group name and a prefix 
to be used when creating projects(terms).

Create projects:

Projects are terms in the choosen vocabulary. Because terms(Projects) have no fields information on
them is stored separately in in the timetracker_projects database table.

User entries:


Project status:

Four project states are set on installation. Status has no UI in this version so any changes must be
made in the database.

Development notes:

Timetracker is based on the Movico (MVC OOP) architecture. This allows for easy debugging and modifications.


Time tracker is sponsered by Tactical Technology Collective (http://tacticaltech.org) and 
is distributed under the GNU GENERAL PUBLIC LICENSE Version 3.(http://www.gnu.org/copyleft/gpl.html)

================================================================================

Established in 2003, Tactical Tech is an international NGO working to enable the 
effective use of information for progressive social change. Our work is informed 
by the principles of freedom of expression and freedom of information and the need 
for transparency and accountability.

We are technologists, information designers, data and security experts, and human rights
and environmental justice activists who share a passion for social change.

Our mission is to advance the skills, tools and techniques of rights advocates, 
empowering them to use information and communications to help marginalised communities
understand and effect progressive social, environmental and political change.

