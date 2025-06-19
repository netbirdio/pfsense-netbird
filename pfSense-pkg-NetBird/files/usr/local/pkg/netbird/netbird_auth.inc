<?php

require_once('netbird/netbird.inc');


function netbird_auth_resync_config()
{
    if (!empty($_POST['disconnect'])) {
        $cmd = sprintf('%s down', escapeshellcmd(NETBIRD_BIN));
        exec($cmd);
        return;
    }

    // TODO: Check if setup key or management URL changed
    // TODO: Stop existing service instance if needed

    $cmd = sprintf(
        '%s up -m %s -k %s',
        escapeshellcmd(NETBIRD_BIN),
        escapeshellarg($_POST['managementurl']),
        escapeshellarg($_POST['setupkey'])
    );
    exec($cmd);
}


function netbird_auth_validate_input($post, &$input_errors)
{
    $managementurl = $post['managementurl'];
    if (!empty($managementurl) && !is_URL($managementurl)) {
        $input_errors[] = sprintf(gettext('Management URL (%s) is not a valid URL.'), $managementurl);
    }
}

?>

