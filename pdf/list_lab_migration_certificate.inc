<?php
function _list_lab_migration_certificates()
{
	global $user;
	$query_id = db_query("SELECT id FROM lab_migration_proposal WHERE approval_status=3 AND uid= :uid", array(
		':uid' => $user->uid
	));
	$exist_id = $query_id->fetchObject();
	//var_dump($exist_id->id);die;
	if ($exist_id){
	if ($exist_id->id) {
		if ($exist_id->id < 2) {
			drupal_set_message('<strong>You need to propose a <a href="https://esim.fossee.in/lab-migration-project/proposal">Lab Migration Proposal</a></strong> or if you have already proposed then your Lab Migration is under reviewing process', 'status');
			return '';
		} //$exist_id->id < 3
		else {
			$search_rows = array();
			global $output;
			$output = '';
			$query3 = db_query("SELECT id,lab_title,name FROM lab_migration_proposal WHERE approval_status=3 AND uid= :uid", array(
				':uid' => $user->uid
			));
			while ($search_data3 = $query3->fetchObject()) {
				if ($search_data3->id) {
					$search_rows[] = array(
						$search_data3->lab_title,
						$search_data3->name,
						l('Download Certificate', 'lab_migration/certificates/generate-pdf/' . $search_data3->id)
					);
				} //$search_data3->id
			} //$search_data3 = $query3->fetchObject()
			if ($search_rows) {
				$search_header = array(
					'Project Title',
					'Contributor Name',
					'Download Certificates'
				);
				$output        = theme('table', array(
					'header' => $search_header,
					'rows' => $search_rows
				));
				return $output;
			} //$search_rows
			else {
				echo ("Error");
				return '';
			}
		}
		}
	} //$exist_id->id
	else {
		drupal_set_message('<strong>You need to propose a <a href="https://esim.fossee.in/lab-migration-project/proposal">Lab Migration Proposal</a></strong> or if you have already proposed then your Lab Migration is under reviewing process', 'status');
		$page_content = "<span style='color:red;'> No certificate available </span>";
		return $page_content;
	}
}
