
Setting up zohoCRM module
-------------------------


1. Download zohocrm library from github. 
2. configure Zoho CRM API auth token at admin/settings/zohocrm/settings
3. Add (create) field mapping
4. Configure fields for field mapping at admin/settings/zohocrm/mapping/<mapping_name>/fields
5. Install and configure rules module to trigger sending of Drupal data to Zoho every time user object (or node) is saved.

Note: zohoCRM module required curl/php-curl installed to work. WSOD if php-curl is not installed.


Testing Install
---------------
Assuming you have zohocrm, zohocrm_user and zohocrm_node enabled, PHP library installed and auth token configured.

1. Go to admin/settings/zohocrm/mapping/add and create field mapping:
      Mapping Name: user_contact
      Mapping Description: Maps Drupal user object to Zoho's Contact
      Drupal Endpoint: User Account
      Zoho Module: Contacts

2. Go to admin/settings/zohocrm/mapping/user_contact/fields and map the fields in the "Drupal to Zoho" fieldset
      Email => E-mail
      First Name => Username
      Last Name => Username

3. Create content type "profile" and add two CCK textfields (assuming CCK module has been installed previously):
      First Name
      Last Name

4. Install content_profile module
  Need to enable all the (sub) modules content_profile_tokens, content_profile_registration. Also need to install 
  token module

5. Go to admin/content/types/profile and check the checkbox "Use this content type as a content profile for users"

6. Go to admin/settings/zohocrm/mapping/add and create field mapping:
      Mapping Name: profile_contact
      Mapping Description: Maps profile node to Zoho's Contact
      Drupal Endpoint: Content Type: Profile
      Zoho Module: Contacts

7. Go to admin/settings/zohocrm/mapping/profile_contact/fields and map the fields in the "Drupal to Zoho" fieldset
      First Name => First Name
      Last Name => Last Name

8. Install rules module

9. Create rule with the following properties:
      Event: User - User account has been created
      Action: Zoho CRM - Send user account data to Zoho (select mapping user_contact)

10. Create rule with the following properties:
      Event: User - User account details have been updated
      Action: Zoho CRM - Send user account data to Zoho (select mapping user_contact)

11. Create rule with the following properties:
      Event: Node - After saving new content
      Condition: Node - content has type (select Profile content type)
      Action: Zoho CRM - Send node data to Zoho (select mapping profile_contact)

12. Create rule with the following properties:
      Event: Node - After updating existing content
      Condition: Node - content has type (select Profile content type)
      Action: Zoho CRM - Send node data to Zoho (select mapping profile_contact)

As you can see from the above description, due to lack of custom
fields in Drupal user object we are forced to use content_profile
module if we want to sync Drupal user with Zoho Contact.

If you would like to sync Account data from Drupal to Zoho you would
create content type Account, create CCK fields you want to be synced
to Zoho and then perform similar steps as describe above. However,
this time you would need only one field mapping (Account node to
Zoho Accounts) and two rules (one for node insert and one for node
update event).


13. There is a Drupal variable which you can use to enable debug
messages. Every time data is being sent to Zoho you will get outgoing
data and Zoho response printed on the page as drupal message as well
as logged in the file /tmp/drupal_debug.txt. This functionality
depends on devel module so make sure devel is enabled before you
enable the variable. There is no UI for configuring this variable
but you can use drush to quickly turn it on: 

Enable debug mode:
    drush vset zohocrm_debug_mode 1 

Disable debug mode:
    drush vset zohocrm_debug_mode 0



As far as porting to D7 is concerned, I'm expecting that most parts
of the module code will be quite straightforward to port expect one
essential part - that is CCK fields support. In D7 fields are moved
to core and whatever code I have in D6 to provide support for CCK
fields won't really be portable to D7. Did you look into that so
far? Any thoughts?

Do you keep your D7 port in a sandbox on d.o? I would like to have
a look at some point.

