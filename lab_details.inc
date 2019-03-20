<?php
    function lab_migration_completed_labs_all() {
        $page_content = "";
        //$query = "SELECT * FROM {lab_migration_proposal} WHERE approval_status = 3";
        $query = db_select('lab_migration_proposal');
        $query->fields('lab_migration_proposal');
        $query->condition('approval_status', 3);
        $result = $query->execute();
        //$result = db_query($query);
        $proposal_rows = array();
        $i=1;
            while($row = $result->fetchObject()) {
                $approval_date=date("Y", $row->approval_date);
                $preference_rows[] = array($i, $row->university , l($row->lab_title,"lab_migration_run/".$row->id),$approval_date);
                $i++;
            }
        $preference_header = array('No','Institute', 'Lab', 'Year');
        $output = theme('table', array('header' => $preference_header, 'rows' => $preference_rows));
        return $output;
    }                                                                           

    function lab_migration_labs_progress_all() {
        $page_content = "";
        //$query = "SELECT * FROM {lab_migration_proposal} WHERE approval_status = 1 and solution_status = 2";
        $query = db_select('lab_migration_proposal');
        $query->fields('lab_migration_proposal');
        $query->condition('approval_status', 1);
        $query->condition('solution_status', 2);
        $result = $query->execute();
        //$result = db_query($query);
        $page_content .= "<ol>";
        while($row = $result->fetchObject()) {
            $page_content .= "<li>";
            $page_content .= $row->university . " ({$row->lab_title})";
            $page_content .= "</li>";
        }
        $page_content .= "</ol>";
        return $page_content;
    }
?>
