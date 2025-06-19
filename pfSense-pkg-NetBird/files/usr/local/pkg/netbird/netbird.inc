<?php

define('NETBIRD_BIN', '/usr/local/bin/netbird');
define('PKG_BIN', '/usr/sbin/pkg');

function netbird_get_pkg_info($field_map = NULL, $packages = NULL)
{
    $pkg_delm = "\t";

    $cmd = implode(' ', [PKG_BIN, 'query', escapeshellarg(implode($pkg_delm, $field_map)), implode(' ', $packages)]);

    exec($cmd, $out, $rc);

    if ($rc !== 0)
        return false;

    $return_keys = array_values(array_flip($field_map));
    foreach ($out as $row_idx => $row_string)
        foreach (explode($pkg_delm, $row_string) as $field_idx => $field_string)
            $ret[$row_idx][$return_keys[$field_idx]] = $field_string;

    return $ret;
}

?>