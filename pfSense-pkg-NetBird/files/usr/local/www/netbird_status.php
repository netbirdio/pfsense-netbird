<?php

require_once('guiconfig.inc');
require_once('util.inc');


$tab_array = [];
$tab_array[] = [gettext('Authentication'), false, 'pkg_edit.php?xml=netbird/netbird_auth.xml'];
$tab_array[] = [gettext('Settings'), false, 'pkg_edit.php?xml=netbird.xml'];
$tab_array[] = [gettext('Status'), true, '/netbird_status.php'];

include('head.inc');

display_top_tabs($tab_array);
?>

    <div class="panel panel-default">
        <div class="panel-heading">
            <h2 class="panel-title"><?=gettext("NetBird Status")?>
            </h2>
        </div>
<!--        <div class="panel-body">-->
<!--            <p>TOStatus</p>-->
<!--        </div>-->
    </div>

<?php
include('foot.inc');


