<?php

require_once('guiconfig.inc');
require_once('util.inc');
require_once('netbird/netbird.inc');

$tab_array = [];
$tab_array[] = [gettext('Authentication'), false, 'pkg_edit.php?xml=netbird/netbird_auth.xml'];
$tab_array[] = [gettext('Settings'), false, 'pkg_edit.php?xml=netbird.xml'];
$tab_array[] = [gettext('Status'), true, '/netbird_status.php'];

$pgtitle = [gettext('Status'), gettext('NetBird')];
$pglinks = ['', '@self'];

$pkg_field_map = ['name' => '%n', 'version' => '%v', 'comment' => '%c'];
$pkg_packages = ['pfSense-pkg-netBird', 'netbird'];

include('head.inc');

display_top_tabs($tab_array);
?>

    <div class="panel panel-default">
        <div class="panel-heading">
            <h2 class="panel-title"><?=gettext("NetBird Status")?>
            </h2>
        </div>
        <div class="table-responsive">
            <p>TODO: render status report</p>
        </div>
    </div>

    <div class="panel panel-default">
        <div class="panel-heading">
            <h2 class="panel-title"><?=gettext('Package Versions')?></h2>
        </div>
        <div class="table-responsive panel-body">
            <table class="table table-hover table-striped table-condensed">
                <thead>
                <tr>
                    <th><?=gettext('Name')?></th>
                    <th><?=gettext('Version')?></th>
                    <th><?=gettext('Comment')?></th>
                </tr>
                </thead>
                <tbody>
                <?php foreach (netbird_get_pkg_info($pkg_field_map, $pkg_packages) as $package): ?>
                    <tr>
                        <td><?=htmlspecialchars($package['name'])?></td>
                        <td><?=htmlspecialchars($package['version'])?></td>
                        <td><?=htmlspecialchars($package['comment'])?></td>
                    </tr>
                <?php endforeach; ?>
                </tbody>
            </table>
        </div>
    </div>

<?php
include('foot.inc');


