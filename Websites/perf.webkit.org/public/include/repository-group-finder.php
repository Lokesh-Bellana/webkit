<?php

class RepositoryGroupFinder
{

    function __construct($db, $triggerable_id) {
        $this->db = $db;
        $this->triggerable_id = $triggerable_id;
        $this->repositories_by_group = NULL;
    }

    function find_by_repositories($repositories)
    {
        if ($this->repositories_by_group === NULL)
            $this->populate_map();
        sort($repositories, SORT_NUMERIC);
        foreach ($this->repositories_by_group as $group_id => $group_repositories) {
            if (count($repositories) == count($group_repositories) && !array_diff($repositories, $group_repositories))
                return $group_id;
        }
        return NULL;
    }

    private function populate_map()
    {
        $repository_rows = $this->db->query_and_fetch_all('SELECT * FROM triggerable_repositories WHERE trigrepo_group IN
            (SELECT repositorygroup_id FROM triggerable_repository_groups WHERE repositorygroup_triggerable = $1)
            ORDER BY trigrepo_group, trigrepo_repository', array($this->triggerable_id));
        if ($repository_rows === FALSE)
            exit_with_error('FailedToFetchRepositoryGroups', array('triggerable' => $this->triggerable_id));

        $repositories_by_group = array();
        foreach ($repository_rows as &$row) {
            $group_id = $row['trigrepo_group'];
            array_ensure_item_has_array($repositories_by_group, $group_id);
            array_push($repositories_by_group[$group_id], $row['trigrepo_repository']);
        }

        $this->repositories_by_group = &$repositories_by_group;
    }
}

?>
