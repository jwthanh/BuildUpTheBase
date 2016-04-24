import argparse
import sys
from apiclient import sample_tools
from oauth2client import client

import dominate
from dominate import tags

# Declare command-line flags.
argparser = argparse.ArgumentParser(add_help=False)
argparser.add_argument('package_name',
                       help='The package name. Example: com.android.sample')


def main(argv):
  # Authenticate and construct service.
  service, flags = sample_tools.init(
      argv,
      'androidpublisher',
      'v2',
      __doc__,
      __file__,
      parents=[argparser],
      scope='https://www.googleapis.com/auth/androidpublisher')

  # Process flags and read their values.
  package_name = flags.package_name

  try:

    edit_request = service.edits().insert(body={}, packageName=package_name)
    result = edit_request.execute()
    edit_id = result['id']

    apks_result = service.edits().apks().list(
        editId=edit_id, packageName=package_name).execute()

    version_codes = [apk['versionCode'] for apk in apks_result['apks']]

    doc = dominate.document(title="Build Up The Base! Changelog")
    with doc.head:
        tags.script(type='text/javascript', src='https://ajax.googleapis.com/ajax/libs/jquery/2.2.2/jquery.min.js')
        tags.link(rel='stylesheet', href='https://ajax.googleapis.com/ajax/libs/jqueryui/1.11.4/themes/smoothness/jquery-ui.css')
        tags.script(type='text/javascript', src='https://ajax.googleapis.com/ajax/libs/jqueryui/1.11.4/jquery-ui.min.js')
        tags.script("""
        $(function(){
            $('#change_log').accordion();
        });
        """,type='text/javascript')

    with doc:
        with tags.div(id="change_log"):
            for version in version_codes[::-1]:
                listings_result = service.edits().apklistings().list(
                        editId=edit_id, packageName=package_name, apkVersionCode=version
                    ).execute()

                tags.h3("VERSION %s" % version, id="version_%s"%version)
                with tags.div():
                    if 'listings' in listings_result:
                        for line in  listings_result['listings'][0]['recentChanges'].splitlines():
                            tags.div(line)
                    else:
                        tags.div("No listing found")

    print doc

  except client.AccessTokenRefreshError:
    print ('The credentials have been revoked or expired, please re-run the '
           'application to re-authorize')

if __name__ == '__main__':
  main(sys.argv)
